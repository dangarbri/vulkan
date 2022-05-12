#include "validation_layers.h"
#include <vector>
#include <stdexcept>
#include <iostream>
#include <cstring>

#ifndef NDEBUG
void ValidationLayers::enableValidationLayers(VkInstanceCreateInfo* info) {
  if (!checkValidationLayerSupport(validationLayers)) {
    throw std::runtime_error("The specified validation layers are not supported");
  }

  std::cout << "Enabling validation layers" << std::endl;
  info->enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
  info->ppEnabledLayerNames = validationLayers.data();
}

bool ValidationLayers::checkValidationLayerSupport(const std::vector<const char*> validationLayers) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
      bool layerFound = false;

      for (const auto& layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }

      if (!layerFound) {
        std::cout << "Validation layer " << layerName << " is not supported." << std::endl;
        return false;
      }
    }

    return true;
}
#endif
