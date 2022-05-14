#include "valium_swapchain.h"
#include "valium_queue.h"
#include <iostream>
#include <algorithm>

/**
 * Queries for available presentation modes
 */
static std::vector<VkPresentModeKHR> GetPresentationModes(VkPhysicalDevice device, VkSurfaceKHR surface);

/**
 * Queries the device for available surface formats
 */
static std::vector<VkSurfaceFormatKHR> GetSurfaceFormatDetails(VkPhysicalDevice device, VkSurfaceKHR surface);

/**
 * Queries for capabilities
 */
static VkSurfaceCapabilitiesKHR GetCapabilities(VkPhysicalDevice device, VkSurfaceKHR surface);

/**
 * Checks that the most common color format is available
 */
static bool SupportsBGRA_SRGB_Nonlinear(VkPhysicalDevice device, VkSurfaceKHR surface);

struct ValiumSwapchain::ValiumSwapchainImpl {
  /** Vulkan logical device */
  VkDevice logicalDevice;
     
  /** Device that will be used for queries */
  VkPhysicalDevice device;

  /** Surface that will be used for queries */
  VkSurfaceKHR surface;

  /** Indices to pass through to the swapchain */
  uint32_t queueFamilyIndices[2];

  /** Holds handles to images in the swapChain */
  std::vector<VkImage> swapChainImages;

  /** The swapchain created by ValiumSwapchain::InitializeSwapchain() */
  VkSwapchainKHR swapChain = VK_NULL_HANDLE;

  /** Swapchain's extent, saved when swapchain is created with InitializeSwapchain() */
  VkExtent2D extent;

  /** Format for images stored on the swapchain. */
  VkFormat imageFormat = VK_FORMAT_B8G8R8A8_SRGB;

  ValiumSwapchainImpl(VkPhysicalDevice device, VkSurfaceKHR surface, VkDevice logicalDevice) : device{device}, surface{surface}, logicalDevice(logicalDevice) {}

  /**
   * Returns a VkExtent2D to be used when constructing a swapchain
   *
   * @param[in] width Window width
   * @param[in] height Window height
   */
  VkExtent2D GetExtent(uint32_t width, uint32_t height);

  /**
   * Returns the number of images to include in the swapchain
   */
  uint32_t GetSwapchainImageCount();

  /**
   * Determines if the imageSharing mode should be concurrent or exclusive and
   * sets that information in the given createinfo.
   *
   * @param[out] createInfo Swapchain creation struct to set imageSharingMode values into.
   */
  void SetImageSharingMode(VkSwapchainCreateInfoKHR &createInfo);

  /**
   * Gets image handles from the swapchain after its creation.
   * @note Call after InitializeSwapchain()
   *
   * Initializes swapChainImages;
   */
  void LoadImageHandles();
};


static std::vector<VkSurfaceFormatKHR> GetSurfaceFormatDetails(VkPhysicalDevice device, VkSurfaceKHR surface) {
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

static std::vector<VkPresentModeKHR> GetPresentationModes(VkPhysicalDevice device, VkSurfaceKHR surface) {
  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

  std::vector<VkPresentModeKHR> presentModes(presentModeCount);
  if (presentModeCount != 0) {
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes.data());
  }

  return presentModes;
}

static VkSurfaceCapabilitiesKHR GetCapabilities(VkPhysicalDevice device, VkSurfaceKHR surface) {
  VkSurfaceCapabilitiesKHR capabilities;
  
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);

  return capabilities;
}

static bool SupportsBGRA_SRGB_Nonlinear(VkPhysicalDevice device, VkSurfaceKHR surface) {
  std::vector<VkSurfaceFormatKHR> availableFormats = GetSurfaceFormatDetails(device, surface);
  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        return true;
    }
  }

  return false;
}

VkExtent2D ValiumSwapchain::ValiumSwapchainImpl::GetExtent(uint32_t width, uint32_t height) {
  VkSurfaceCapabilitiesKHR capabilities = GetCapabilities(device, surface);
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    VkExtent2D actualExtent = {
      width,
      height
    };

    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return actualExtent;
  }
}

ValiumSwapchain::ValiumSwapchain(VkPhysicalDevice device, VkSurfaceKHR surface, VkDevice logicalDevice) {
  _impl = new ValiumSwapchainImpl{device, surface, logicalDevice};
}

ValiumSwapchain::~ValiumSwapchain() {
  if (_impl->swapChain != VK_NULL_HANDLE) {
    std::cout << "Destroying the swapchain." << std::endl;
    vkDestroySwapchainKHR(_impl->logicalDevice, _impl->swapChain, nullptr);
  }

  delete _impl;
}

// static
bool ValiumSwapchain::SupportsDrawing(VkPhysicalDevice device, VkSurfaceKHR surface) {
  std::vector<VkPresentModeKHR> modes = GetPresentationModes(device, surface);

  bool has_modes = modes.size() > 0;
  bool has_desired_format = SupportsBGRA_SRGB_Nonlinear(device, surface);

  return has_modes && has_desired_format;
}

void ValiumSwapchain::InitializeSwapchain(uint32_t width, uint32_t height) {
  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = _impl->surface;

  uint32_t imageCount = _impl->GetSwapchainImageCount();
  createInfo.minImageCount = imageCount;
  
  // SupportsDrawing should have been called first to confirm this support
  createInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
  createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  
  VkExtent2D extent = _impl->GetExtent(width, height);
  createInfo.imageExtent = extent;
  _impl->extent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  _impl->SetImageSharingMode(createInfo);

  auto capabilities = GetCapabilities(_impl->device, _impl->surface);
  createInfo.preTransform = capabilities.currentTransform;

  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
  // Don't render pixels covered by other windows by setting clipped = VK_TRUE
  createInfo.clipped = VK_TRUE;
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  std::cout << "Creating the swapchain." << std::endl;
  if (vkCreateSwapchainKHR(_impl->logicalDevice, &createInfo, nullptr, &_impl->swapChain) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }
  _impl->LoadImageHandles();
}

void ValiumSwapchain::ValiumSwapchainImpl::SetImageSharingMode(VkSwapchainCreateInfoKHR &createInfo) {
  QueueFamilyIndices indices = ValiumQueue::GetQueueIndices(device, surface);
  queueFamilyIndices[0] = indices.graphicsFamily.value();
  queueFamilyIndices[1] = indices.presentFamily.value();

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
  }
}

uint32_t ValiumSwapchain::ValiumSwapchainImpl::GetSwapchainImageCount() {
  VkSurfaceCapabilitiesKHR capabilities = GetCapabilities(device, surface);

  uint32_t imageCount = capabilities.minImageCount + 1;
  if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
    imageCount = capabilities.maxImageCount;
  }

  return imageCount;
}

void ValiumSwapchain::ValiumSwapchainImpl::LoadImageHandles() {
  uint32_t imageCount;
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
  swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
}
