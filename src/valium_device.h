#pragma once

#include <vulkan/vulkan.h>

/**
 * @brief Encapsulates a logical device to be used with Vulkan
 *
 * Originally Valium was going to be the only interface for interacting with
 * vulkan, but as I learned more about the complexity of vulkan, I realized
 * it's going to take more than one class to neatly encapsulate the required
 * functionality.
 *
 * @todo Consider using this to encapsulate both physical and logical devices
 */
class ValiumDevice
{
public:
  /**
   * Creates a logical device to interface with the given physical @a device
   * @param[in] device Reference to the physical vulkan device
   * @param[in] surface Surface that this device will be drawing to
   * @param[in] width Surface width
   * @param[in] height Surface height
   **/
  ValiumDevice(const VkPhysicalDevice device, const VkSurfaceKHR surface, const uint32_t width, const uint32_t height);
  ~ValiumDevice();

  /**
   * Checks if the device supports the default required extensions
   * for use with valium
   *
   * @param[in] device The device to check support on.
   */
  static bool SupportsRequiredExtensions(VkPhysicalDevice device);

private:
  struct ValiumDeviceImpl;
  ValiumDeviceImpl* _impl;
};
