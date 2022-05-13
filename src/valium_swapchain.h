#pragma once

#include <vulkan/vulkan.h>
#include <vector>

/**
 * Can be used to gather information about a swapchain
 */
class ValiumSwapchain
{
 public:
  /**
   * Enables functions that will query the given device and surface pair
   * for various features.
   *
   * @param[in] device Device to query
   * @param[in] surface Surface to query support for
   */
  ValiumSwapchain(VkPhysicalDevice device, VkSurfaceKHR surface);
  ~ValiumSwapchain();

  /**
   * Queries the swapchain and returns true if the given @a device and @a surface
   * pair support are usable.
   *
   * @returns true if there is at least one presentation mode and surface format.
   */
  bool SupportsDrawing();
 private:
  struct ValiumSwapchainImpl;
  ValiumSwapchainImpl* _impl;
};

