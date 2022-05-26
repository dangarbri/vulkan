#include "valium_command_pool.h"
#ifdef SHOW_RESOURCE_ALLOCATION
#include <iostream>
#endif

struct ValiumCommandPool::impl {
  /** Device used for submitting commands to */
  VkDevice _device;

  /** Command pool */
  VkCommandPool _pool = VK_NULL_HANDLE;

  /** Command buffer to be used in the pool */
  VkCommandBuffer _buffer;

  /**
   * Construts the command pool with the graphics family index.
   *
   * @param[in] indices Queue indices that contains a graphics family
   */
  void CreateCommandPool(QueueFamilyIndices indices);

  /**
   * Allocates a command buffer from ValiumCommandPool::impl::_pool
   */
  void AllocateCommandBuffer();
};

ValiumCommandPool::ValiumCommandPool(VkDevice device, QueueFamilyIndices indices) {
  _impl = new impl();
  _impl->CreateCommandPool(indices);
  _impl->AllocateCommandBuffer();
}

ValiumCommandPool::~ValiumCommandPool() {
  if (_impl->_pool != VK_NULL_HANDLE) {
#ifdef SHOW_RESOURCE_ALLOCATION
    std::cout << "Freeing command pool" << std::endl;
#endif
    vkDestroyCommandPool(_impl->_device, _impl->_pool, nullptr);
  }
  delete _impl;
}

void ValiumCommandPool::impl::CreateCommandPool(QueueFamilyIndices indices) {
  VkCommandPoolCreateInfo poolInfo{};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = indices.graphicsFamily.value();
#ifdef SHOW_RESOURCE_ALLOCATION
  std::cout << "Creating command pool" << std::endl;
#endif
  if (vkCreateCommandPool(_device, &poolInfo, nullptr, &_pool) != VK_SUCCESS) {
    throw std::runtime_error("failed to create command pool!");
  }
}

void ValiumCommandPool::impl::AllocateCommandBuffer() {
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = _pool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = 1;

  if (vkAllocateCommandBuffers(_device, &allocInfo, &_buffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to allocate command buffers!");
  }
}

VkCommandBuffer ValiumCommandPool::RecordCommand(uint32_t imgIndex) {
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0; // Optional
  beginInfo.pInheritanceInfo = nullptr; // Optional

  if (vkBeginCommandBuffer(_impl->_buffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }

  return _impl->_buffer;
}
