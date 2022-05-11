#ifndef _VALIUM_H_
#define _VALIUM_H_

/**
 * Valium is the Vulkan Abstraction Layer.
 * This may be extended as needed to expose required Vulkan API features
 * as needed.
 *
 * Since Vulkan is a low level API, this file is meant to encapsulate working
 * with the API.
 */

#include <memory>
#include <vector>
#include <string>

class Valium
{
 public:
  Valium(const char* app_name);
  ~Valium();

  /**
   * Gets the available extensions from vulkan and returns them in a vector
   */
  std::vector<std::string> GetAvailableExtensions();
 private:
  struct impl;
  impl* _impl;
};

#endif
