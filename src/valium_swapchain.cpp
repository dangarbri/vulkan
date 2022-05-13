#include "valium_swapchain.h"

struct ValiumSwapchain::ValiumSwapchainImpl {
  /** Device that will be used for queries */
  VkPhysicalDevice device;

  /** Surface that will be used for queries */
  VkSurfaceKHR surface;

  ValiumSwapchainImpl(VkPhysicalDevice device, VkSurfaceKHR surface) : device{device}, surface{surface} {}
  
  /**
   * Queries the device for available surface formats
   */
  std::vector<VkSurfaceFormatKHR> GetSurfaceFormatDetails();

  /**
   * Queries for available presentation modes
   */
  std::vector<VkPresentModeKHR> GetPresentationModes();

  /**
   * Queries for capabilities
   */
  VkSurfaceCapabilitiesKHR GetCapabilities();
  
};


std::vector<VkSurfaceFormatKHR> ValiumSwapchain::ValiumSwapchainImpl::GetSurfaceFormatDetails() {
  // Get count
  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  // Get the actual formats supported if any
  std::vector<VkSurfaceFormatKHR> formats(formatCount);
  if (formatCount != 0) {
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats.data());
  }

  return formats;
}

std::vector<VkPresentModeKHR> ValiumSwapchain::ValiumSwapchainImpl::GetPresentationModes() {
  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

  std::vector<VkPresentModeKHR> presentModes(presentModeCount);
  if (presentModeCount != 0) {
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes.data());
  }

  return presentModes;
}

VkSurfaceCapabilitiesKHR ValiumSwapchain::ValiumSwapchainImpl::GetCapabilities() {
  VkSurfaceCapabilitiesKHR capabilities;
  
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

  return capabilities;
}

ValiumSwapchain::ValiumSwapchain(VkPhysicalDevice device, VkSurfaceKHR surface) {
  _impl = new ValiumSwapchainImpl{device, surface};

  /*
  SwapchainSupportDetails details;

  // Query capabilities ans save them to capabilities


  // Get format details
  details.formats = GetSurfaceFormatDetails(device, surface);
  return details; */
}

ValiumSwapchain::~ValiumSwapchain() {
  delete _impl;
}

bool ValiumSwapchain::SupportsDrawing() {
  std::vector<VkPresentModeKHR> modes = _impl->GetPresentationModes();
  std::vector<VkSurfaceFormatKHR> formats = _impl->GetSurfaceFormatDetails();

  bool has_modes = modes.size() > 0;
  bool has_formats = formats.size() > 0;

  return has_modes && has_formats;
}
