#pragma once

#include <vulkan/vulkan.h>

/**
 * Holds a view to an image on the swapchain that can be used for rendering
 */
class ValiumView
{
 public:
  ValiumView(VkDevice device, VkImage image);
  ~ValiumView();

  /**
   * Returns the vulkan view that this wraps
   */
  VkImageView GetVkImageView();
 private:
  struct impl;
  impl* _impl;
};
