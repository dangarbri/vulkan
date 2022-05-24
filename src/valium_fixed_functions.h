#pragma once

#include <vulkan/vulkan.h>

/**
 * Stores default parameters for the graphic's pipeline's fixed functions
 */
namespace ValiumFixedFnInfo {
  /**
   * Specifies the format of the vertex buffers that will be passed to the pipeline.
   * Note: At this stage there is no vertex input, so it is left empty.
   */
  const VkPipelineVertexInputStateCreateInfo VERTEX_INPUT_INFO {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .vertexBindingDescriptionCount = 0,
    .pVertexBindingDescriptions = nullptr, // Optional
    .vertexAttributeDescriptionCount = 0,
    .pVertexAttributeDescriptions = nullptr
  };

  /**
   * Specifies how vertices will be used.
   * Currently the only mode enabled is triangles. Every group of 3 vertices
   * will be rendered as a triangle.
   */
  const VkPipelineInputAssemblyStateCreateInfo VERTEX_ASSEMBLY_INFO {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE
  };

  /**
   * Contains parameters for the rasterizer.
   *
   * Interesting Parameters:
   *  - polygonMode options are "Fill", "Edges", "Points" presumably
   *                these tell the rasterizer how it should draw the vertices given.
   */
  const VkPipelineRasterizationStateCreateInfo RASTERIZER_INFO {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .lineWidth = 1.0f,
    .cullMode = VK_CULL_MODE_BACK_BIT,
    .frontFace = VK_FRONT_FACE_CLOCKWISE,
    .depthBiasEnable = VK_FALSE,
    .depthBiasConstantFactor = 0.0f, // Optional
    .depthBiasClamp = 0.0f, // Optional
    .depthBiasSlopeFactor = 0.0f  // Optional
  };

  /**
   * Default parameters for disabling multisampling
   * @TODO This requires a GPU feature to enable. May be good to find the feature
   *       and enable it if it's available.
   */
  const VkPipelineMultisampleStateCreateInfo MULTISAMPLING_INFO {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .sampleShadingEnable = VK_FALSE,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .minSampleShading = 1.0f, // Optional
    .pSampleMask = nullptr, // Optional
    .alphaToCoverageEnable = VK_FALSE, // Optional
    .alphaToOneEnable = VK_FALSE  // Optional
  };

  /**
   * Defines how colors should be blended in the framebuffer
   */
  const VkPipelineColorBlendAttachmentState COLOR_BLEND_ATTACH_INFO {
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    .blendEnable = VK_FALSE,
    .srcColorBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
    .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
    .colorBlendOp = VK_BLEND_OP_ADD, // Optional
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE, // Optional
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO, // Optional
    .alphaBlendOp = VK_BLEND_OP_ADD  // Optional
  };

  /**
   * Color blending creation info
   */
  const VkPipelineColorBlendStateCreateInfo COLOR_BLEND_INFO {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .logicOpEnable = VK_FALSE,
    .logicOp = VK_LOGIC_OP_COPY, // Optional
    .attachmentCount = 1,
    .pAttachments = &COLOR_BLEND_ATTACH_INFO,
    .blendConstants[0] = 0.0f, // Optional
    .blendConstants[1] = 0.0f, // Optional
    .blendConstants[2] = 0.0f, // Optional
    .blendConstants[3] = 0.0f  // Optional
  };

  /**
   * Creates a VkViewport with the given width/height parameters
   *
   * @param[in] width Desired viewport width in pixels
   * @param[in] height Desired viewport height in pixels
   * @returns VkViewport
   */
  VkViewport GetViewport(uint32_t width, uint32_t height);

  /**
   * Creates the scissor rect. At this time the rect is only the full screen.
   *
   * @param[in] swapchainExtent The extent that was used to create the swapchain being used.
   * @returns VkRect2D
   */
  VkRect2D GetScissor(VkExtent2D swapchainExtent);

  /**
   * Constructs and returns information used for creating a viewport state
   *
   * @param[in] viewport A viewport created with GetViewport()
   * @param[in] scissor A scissor created with GetScissor()
   */
  VkPipelineViewportStateCreateInfo GetViewportStateCreateInfo(VkViewport &viewport, VkRect2D &scissor);
};
