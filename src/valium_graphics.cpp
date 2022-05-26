#include "valium_graphics.h"
#include "valium_fixed_functions.h"
#include "valium_renderpass.h"
#include <fstream>
#include <vector>
#if SHOW_RESOURCE_ALLOCATION
#include <iostream>
#endif

/**
 * Encapsulates shader information needed for using a shader
 * in the graphics pipeline
 */
struct ShaderInfo {
  /**
   * Compiled and loaded vertex or fragment shader
   */
  VkShaderModule shader;

  /**
   * Information for using the shader in the graphics pipeline
   */
  VkPipelineShaderStageCreateInfo createInfo;
};

struct ValiumGraphics::impl {
  /**
   * Handle to the final constructed graphics pipeline
   */
  VkPipeline _graphicsPipeline = VK_NULL_HANDLE;

  /**
   * Handle to the current device
   */
  VkDevice _device;

  /**
   * Stores the swapchain's extent
   */
  VkExtent2D _extent;

  /**
   * Holds the shader module that will execute our shaders
   */
  std::vector<ShaderInfo> _shaders;

  /**
   * Pipeline layout used for specifying uniform values in the pipeline
   */
  VkPipelineLayout _pipelineLayout = VK_NULL_HANDLE;

  /**
   * Stores the renderpass
   */
  ValiumRenderPass* _renderPass = nullptr;

  /**
   * Reads a binary file into a char buffer
   *
   * @param[in] filename Path to the file to read
   */
  std::vector<char> _ReadFile(const std::string& filename);

  /**
   * Loads a compiled shader from the given file
   *
   * @param[in] shader path to SPIR-V compiled shader
   */
  void _LoadShader(const std::string& shader, VkShaderStageFlagBits type);

  /**
   * Loads a shader module into the graphics pipeline
   *
   * @param[in] shader The shader being loaded
   * @param[in] type Specifies if this is a vertex or fragment shader
   */
  VkPipelineShaderStageCreateInfo _CreateShaderPipelineInfo(VkShaderModule shader, VkShaderStageFlagBits type);

  /**
   * Creates the pipelineLayout info
   */
  void _CreatePipelineLayout();

  /**
   * Constructs the final graphics pipeline
   */
  void _CreateGraphicsPipeline(VkExtent2D extent);
};

ValiumGraphics::ValiumGraphics(VkDevice device, ValiumSwapchain* swapchain) {
  _impl = new impl();
  _impl->_device = device;
  _impl->_extent = swapchain->GetExtent();
  _impl->_CreatePipelineLayout();
  _impl->_renderPass = new ValiumRenderPass(device);
}

ValiumGraphics::~ValiumGraphics() {
  // Destroy the loaded shaders
  for (auto shaderInfo : _impl->_shaders) {
    vkDestroyShaderModule(_impl->_device, shaderInfo.shader, nullptr);
  }

  if (_impl->_pipelineLayout != VK_NULL_HANDLE) {
    vkDestroyPipelineLayout(_impl->_device, _impl->_pipelineLayout, nullptr);
  }

  delete _impl->_renderPass;

  if (_impl->_graphicsPipeline != VK_NULL_HANDLE) {
#if SHOW_RESOURCE_ALLOCATION
    std::cout << "Destroying the graphics pipeline" << std::endl;
#endif
    vkDestroyPipeline(_impl->_device, _impl->_graphicsPipeline, nullptr);
  }

  delete _impl;
}

void ValiumGraphics::LoadShader(const std::string &shader, const VkShaderStageFlagBits type) {
  _impl->_LoadShader(shader, type);
}

void ValiumGraphics::impl::_LoadShader(const std::string& shader, VkShaderStageFlagBits type) {
  // Get the file as a byte buffer
  std::vector<char> code = _ReadFile(shader);

  // Create the struct to create the shader
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

  // Construct the shader
  VkShaderModule shaderModule;
  if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("failed to create shader module!");
  }

  auto info = _CreateShaderPipelineInfo(shaderModule, type);
  // Push the shader into stored memory
  ShaderInfo newShader;
  newShader.shader = shaderModule;
  newShader.createInfo = info;

  _shaders.push_back(newShader);
}

std::vector<char> ValiumGraphics::impl::_ReadFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("failed to open file!");
  }

  size_t fileSize = (size_t) file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);
  file.close();

  return buffer;
}

VkPipelineShaderStageCreateInfo ValiumGraphics::impl::_CreateShaderPipelineInfo(VkShaderModule shader, VkShaderStageFlagBits type) {
  VkPipelineShaderStageCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  createInfo.stage = type;


  createInfo.module = shader;
  createInfo.pName = "main";

  return createInfo;
}

void ValiumGraphics::impl::_CreatePipelineLayout() {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0; // Optional
  pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
  pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
  pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

  if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("failed to create pipeline layout!");
  }
}

void ValiumGraphics::impl::_CreateGraphicsPipeline(VkExtent2D extent) {
  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  // Get the shader stages from the stored shader list
  std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
  for (auto shader : _shaders) {
    shaderStages.push_back(shader.createInfo);
  }
  pipelineInfo.stageCount = shaderStages.size();
  pipelineInfo.pStages = shaderStages.data();

  pipelineInfo.pVertexInputState = &ValiumFixedFnInfo::VERTEX_INPUT_INFO;
  pipelineInfo.pInputAssemblyState = &ValiumFixedFnInfo::VERTEX_ASSEMBLY_INFO;

  // Viewport State
  VkViewport viewport = ValiumFixedFnInfo::GetViewport(extent.width, extent.height);
  VkRect2D scissor = ValiumFixedFnInfo::GetScissor(extent);
  VkPipelineViewportStateCreateInfo viewportStateCreateInfo = ValiumFixedFnInfo::GetViewportStateCreateInfo(viewport, scissor);

  pipelineInfo.pViewportState = &viewportStateCreateInfo;

  pipelineInfo.pRasterizationState = &ValiumFixedFnInfo::RASTERIZER_INFO;
  pipelineInfo.pMultisampleState = &ValiumFixedFnInfo::MULTISAMPLING_INFO;
  pipelineInfo.pDepthStencilState = nullptr; // Optional
  pipelineInfo.pColorBlendState = &ValiumFixedFnInfo::COLOR_BLEND_INFO;
  pipelineInfo.pDynamicState = nullptr; // Optional

  pipelineInfo.layout = _pipelineLayout;
  pipelineInfo.renderPass = _renderPass->GetVkRenderPass();
  pipelineInfo.subpass = 0;

  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
  pipelineInfo.basePipelineIndex = -1; // Optional

  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
  pipelineInfo.basePipelineIndex = -1; // Optional

#if SHOW_RESOURCE_ALLOCATION
  std::cout << "Creating the graphics pipeline" << std::endl;
#endif
  if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
}

void ValiumGraphics::InitializePipeline() {
  _impl->_CreateGraphicsPipeline(_impl->_extent);
}

ValiumRenderPass* ValiumGraphics::GetRenderPass() {
  return _impl->_renderPass;
}
