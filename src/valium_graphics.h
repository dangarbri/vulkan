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
   */
  ValiumGraphics(VkDevice device);
  ~ValiumGraphics();

  /**
   * Load a compiled shader
   *
   * @param[in] shader Path to the compiled shader
   */
  void LoadShader(const std::string& shader);
 private:
  struct impl;
  impl* _impl;
};
