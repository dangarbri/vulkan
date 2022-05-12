#pragma once

#ifndef NDEBUG
#include <vulkan/vulkan.h>
#include <vector>

/**
 * Contains a small set of functions for use with enabling validation
 * layers
 */
namespace ValidationLayers {
  const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
  };

  // Enables validation layers for debugging vulkan usage. To be
  // called when initializing the vulkan instance
  void enableValidationLayers(VkInstanceCreateInfo* info);

  // Checks that the given validation layers are supported by the system
  bool checkValidationLayerSupport(std::vector<const char*> validationLayers);
}
#endif
