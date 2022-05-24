#include "valium_renderpass.h"
#include "valium_swapchain.h" // For swapchain image format
#include <vulkan/vulkan.h>

struct ValiumRenderPass::impl {
  /**
   * Renderpass created on construction
   */
  VkRenderPass _renderPass = VK_NULL_HANDLE;

  /**
   * Logical vulkan device
   */
  VkDevice _device;

  /**
   * Initializes the render pass
   */
  void _CreateRenderPass();
};

ValiumRenderPass::ValiumRenderPass(VkDevice device) {
  _impl = new impl();
  _impl->_device = device;
  _impl->_CreateRenderPass();
}

ValiumRenderPass::~ValiumRenderPass() {
  if (_impl->_renderPass != VK_NULL_HANDLE) {
    vkDestroyRenderPass(_impl->_device, _impl->_renderPass, nullptr);
  }
  delete _impl;
}

void ValiumRenderPass::impl::_CreateRenderPass() {
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = SWAPCHAIN_IMAGE_FORMAT;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS) {
    throw std::runtime_error("failed to create render pass!");
  }
}

VkRenderPass ValiumRenderPass::GetVkRenderPass() const {
  return _impl->_renderPass;
}
