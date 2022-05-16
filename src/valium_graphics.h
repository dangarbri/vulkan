#pragma once

#include <vulkan/vulkan.h>

/**
 * Manages the graphics pipeline
 */
class ValiumGraphics
{
 public:
  /**
   * Initializes a graphics pipeline
   */
  ValiumGraphics();
 private:
  struct impl;
  impl* _impl;
};
