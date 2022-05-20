#include "valium_graphics.h"
#include "valium_fixed_functions.h"
#include <fstream>
#include <vector>

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
   * Handle to the current device
   */
  VkDevice _device;

  /**
   * Holds the shader module that will execute our shaders
   */
  std::vector<ShaderInfo> _shaders;

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
   * Initializes the graphics shader module in the given device
   *
   * @param[in] device Vulkan device
   */
  void _InitializeShaderModule(VkDevice device);
};

ValiumGraphics::ValiumGraphics(VkDevice device) {
  _impl = new impl();
  _impl->_device = device;
}

ValiumGraphics::~ValiumGraphics() {
  // Destroy the loaded shaders
  for (auto shaderInfo : _impl->_shaders) {
    vkDestroyShaderModule(_impl->_device, shaderInfo.shader, nullptr);
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
