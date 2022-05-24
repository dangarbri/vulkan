#pragma once

#include <vulkan/vulkan.h>
#include <vector>

const VkFormat SWAPCHAIN_IMAGE_FORMAT = VK_FORMAT_B8G8R8A8_SRGB;

/**
 * Can be used to gather information about a swapchain and specify
 * swapchain details.
 *
 * The swapchain can be used to apply transforms to your images, such as
 * flips, alpha blending, and rotations. Keep this in mind if you need
 * any of those features.
 */
class ValiumSwapchain
{
 public:
  /**
   * Enables functions that will query the given device and surface pair
   * for various features.
   *
   * @note The swapchain must be manually initialized by calling
   *       InitializeSwapchain().
   *
   * @param[in] device Device to create the swapchain for
   * @param[in] surface Surface that will be drawn to
   * @param[in] logicalDevice The vulkan device created from the physical device
   */
  ValiumSwapchain(VkPhysicalDevice device, VkSurfaceKHR surface, VkDevice logicalDevice);
  ~ValiumSwapchain();

  /**
   * Queries the swapchain and returns true if the given @a device and @a surface
   * pair support are usable.
   *
   * @param[in] device Physical device to query support for
   * @param[in] surface Surface that will be drawn to.
   *
   * @returns true if there is at least one presentation mode and surface format.
   */
  static bool SupportsDrawing(VkPhysicalDevice device, VkSurfaceKHR surface);

  /**
   * Initializes the swapchain with the given resolution
   *
   * @param[in] width Window width
   * @param[in] height Window height
   */
  void InitializeSwapchain(uint32_t width, uint32_t height);

  /**
   * @returns the swapchain image's extent
   */
  VkExtent2D GetExtent();
 private:
  struct ValiumSwapchainImpl;
  ValiumSwapchainImpl* _impl;
};

