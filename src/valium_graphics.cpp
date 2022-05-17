#include "valium_graphics.h"
#include <fstream>
#include <vector>

struct ValiumGraphics::impl {
  /**
   * Handle to the current device
   */
  VkDevice _device;

  /**
   * Holds the shader module that will execute our shaders
   */
  std::vector<VkShaderModule> _shaders;

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
  void _LoadShader(const std::string& shader);

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
  for (auto shader : _impl->_shaders) {
    vkDestroyShaderModule(_impl->_device, shader, nullptr);
  }

  delete _impl;
}

void ValiumGraphics::LoadShader(const std::string &shader) {
  _impl->_LoadShader(shader);
}

void ValiumGraphics::impl::_LoadShader(const std::string& shader) {
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

  // Push the shader into the pipeline storage
  _shaders.push_back(shaderModule);
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
