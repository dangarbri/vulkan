#include <stdexcept>
#include "valium_view.h"
#include "app_config.h"

#if SHOW_RESOURCE_ALLOCATION
#include <iostream>
#endif

struct ValiumView::impl {
  /**
   * Handle to GPU being used
   */
  const VkDevice _device;

  /**
   * Image that this instance will use for operations
   */
  const VkImage _image;

  /**
   * View that will be used in the render pipeline
   */
  VkImageView _imageView = VK_NULL_HANDLE;

  impl(VkDevice device, VkImage image) : _device(device), _image(image) {}

  /**
   * Initializes impl::imageView from impl::image
   */
  void _CreateImageView();
};

ValiumView::ValiumView(VkDevice device, VkImage image) {
  _impl = new impl(device, image);
  _impl->_CreateImageView();
}

ValiumView::~ValiumView() {
  if (_impl->_imageView != VK_NULL_HANDLE) {
#if SHOW_RESOURCE_ALLOCATION
    std::cout << "destroying image view." << std::endl;
#endif 
    vkDestroyImageView(_impl->_device, _impl->_imageView, nullptr);
  }
  delete _impl;
}

void ValiumView::impl::_CreateImageView() {
  VkImageViewCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  createInfo.image = _image;

  createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  createInfo.format = IMAGE_FORMAT;

  createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  createInfo.subresourceRange.baseMipLevel = 0;
  createInfo.subresourceRange.levelCount = 1;
  createInfo.subresourceRange.baseArrayLayer = 0;
  createInfo.subresourceRange.layerCount = 1;

#if SHOW_RESOURCE_ALLOCATION
  std::cout << "Creating image view." << std::endl;
#endif 

  if (vkCreateImageView(_device, &createInfo, nullptr, &_imageView) != VK_SUCCESS) {
    throw std::runtime_error("failed to create image views!");
  }
}
