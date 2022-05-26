#pragma once

#include <vulkan/vulkan.h>
#include "valium_queue.h"

/**
 * Manages the vulkan command queue.
 * Upon constructing this class, a command pool and a single command buffer
 * will be allocated.
 */
class ValiumCommandPool
{
 public:
  /**
   * Constructs a command pool on the given device
   *
   * @param[in] device Device to create the command pool for
   * @param[in] indices Queue family indices that contains the graphicsFamily index
   */
  ValiumCommandPool(VkDevice device, QueueFamilyIndices indices);
  ~ValiumCommandPool();

  /**
   * Records a command into a buffer of the command pool
   *
   * @param[in] imgIndex index of the swapchain image to write to.
   * @returns The command buffer to be passed to the render pass
   */
  VkCommandBuffer RecordCommand(uint32_t imgIndex);
 private:
  struct impl;
  impl* _impl;
};
