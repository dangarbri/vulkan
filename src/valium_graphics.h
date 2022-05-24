#pragma once

#include <vulkan/vulkan.h>
#include <string>

/**
 * Manages the graphics pipeline
 */
class ValiumGraphics
{
 public:
  /**
   * Initializes a graphics pipeline
   *
   * @param[in] device Device to create the graphics pipeline on.
   * @param[in] extent The surface extent
   */
  ValiumGraphics(VkDevice device, VkExtent2D extent);
  ~ValiumGraphics();

  /**
   * Load a compiled shader
   *
   * @param[in] shader Path to the compiled shader
   */
  void LoadShader(const std::string& shader, VkShaderStageFlagBits type);

  /**
   * Create the graphics pipeline, do this after loading shaders
   */
  void InitializePipeline();

 private:
  struct impl;
  impl* _impl;
};
