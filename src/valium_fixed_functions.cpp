#include "valium_fixed_functions.h"

using namespace ValiumFixedFnInfo;

VkViewport ValiumFixedFnInfo::GetViewport(uint32_t width, uint32_t height) {
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float) width;
  viewport.height = (float) height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  return viewport;
}

VkRect2D ValiumFixedFnInfo::GetScissor(VkExtent2D extent) {
  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent;

  return scissor;
}

VkPipelineViewportStateCreateInfo ValiumFixedFnInfo::GetViewportStateCreateInfo(VkViewport &viewport, VkRect2D &scissor) {
  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;

  return viewportState;
}
