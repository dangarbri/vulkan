#pragma once

#include <vulkan/vulkan.h>

/**
 * Manages the framebuffer attachments used for rendering
 */
class ValiumRenderPass {
public:
  ValiumRenderPass(VkDevice device);
  ~ValiumRenderPass();

  /**
   * @returns The vulkan VkRenderPass built by this instance
   */
  VkRenderPass GetVkRenderPass();
 private:
  struct impl;
  impl* _impl;
};
