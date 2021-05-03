/*
Copyright (C) 2019, NVIDIA CORPORATION. All rights reserved.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "vkpt.h"
#include "shader/vertex_buffer.h"

#include <float.h>

VkPipelineLayout        pipeline_layout_smap;
VkRenderPass            render_pass_smap;
VkPipeline              pipeline_smap;
static VkFramebuffer           framebuffer_smap;
static VkFramebuffer           framebuffer_smap2;
static VkImage                 img_smap;
static VkImageView             imv_smap_depth;
static VkImageView             imv_smap_depth2;
static VkImageView             imv_smap_depth_array;
static VkDeviceMemory          mem_smap;

static void
create_render_pass()
{
	LOG_FUNC();
	VkAttachmentDescription depth_attachment = {
		.format         = VK_FORMAT_D32_SFLOAT,
		.samples        = VK_SAMPLE_COUNT_1_BIT,
		.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	};

	VkAttachmentReference depth_attachment_ref = {
		.attachment = 0, /* index in fragment shader */
		.layout     = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR,
	};

	VkSubpassDescription subpass = {
		.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.pDepthStencilAttachment = &depth_attachment_ref,
	};

	// C++20 VKPT: Order fix.
	VkSubpassDependency dependencies[] = {
		{
			.srcSubpass    = VK_SUBPASS_EXTERNAL,
			.dstSubpass    = 0, /* index for own subpass */
			.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = 0, /* XXX verify */
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
			               | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		},
	};

	VkRenderPassCreateInfo render_pass_info = {
		.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments    = &depth_attachment,
		.subpassCount    = 1,
		.pSubpasses      = &subpass,
		.dependencyCount = LENGTH(dependencies),
		.pDependencies   = dependencies,
	};

	_VK(vkCreateRenderPass(qvk.device, &render_pass_info, NULL, &render_pass_smap));
	ATTACH_LABEL_VARIABLE(render_pass_smap, RENDER_PASS);
}

VkResult
vkpt_shadow_map_initialize()
{
	create_render_pass();

	VkPushConstantRange push_constant_range = {
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.offset = 0,
		.size = sizeof(float) * 16,
	};

	CREATE_PIPELINE_LAYOUT(qvk.device, &pipeline_layout_smap,
		.pushConstantRangeCount = 1,
		.pPushConstantRanges = &push_constant_range,
	);


	// C++20 VKPT: Order fix.
	VkImageCreateInfo img_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = VK_FORMAT_D32_SFLOAT,
		.extent = {
			.width = SHADOWMAP_SIZE,
			.height = SHADOWMAP_SIZE,
			.depth = 1,
		},
		.mipLevels = 1,
		.arrayLayers = 2,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
		       | VK_IMAGE_USAGE_SAMPLED_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = (uint32_t)qvk.queue_idx_graphics, // C++20 VKPT: Added cast.
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	_VK(vkCreateImage(qvk.device, &img_info, NULL, &img_smap));
	ATTACH_LABEL_VARIABLE(img_smap, IMAGE);

	VkMemoryRequirements mem_req;
	vkGetImageMemoryRequirements(qvk.device, img_smap, &mem_req);

	_VK(allocate_gpu_memory(mem_req, &mem_smap));

	_VK(vkBindImageMemory(qvk.device, img_smap, mem_smap, 0));

	// C++20 VKPT: Order fix.
	VkImageViewCreateInfo img_view_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = img_smap,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = VK_FORMAT_D32_SFLOAT,
		.components = {
			VK_COMPONENT_SWIZZLE_R,
			VK_COMPONENT_SWIZZLE_G,
			VK_COMPONENT_SWIZZLE_B,
			VK_COMPONENT_SWIZZLE_A,
		},
		.subresourceRange = {
			.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};
	_VK(vkCreateImageView(qvk.device, &img_view_info, NULL, &imv_smap_depth));
	ATTACH_LABEL_VARIABLE(imv_smap_depth, IMAGE_VIEW);

	img_view_info.subresourceRange.baseArrayLayer = 1;
	_VK(vkCreateImageView(qvk.device, &img_view_info, NULL, &imv_smap_depth2));
	ATTACH_LABEL_VARIABLE(imv_smap_depth2, IMAGE_VIEW);

	img_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
	img_view_info.subresourceRange.baseArrayLayer = 0;
	img_view_info.subresourceRange.layerCount = 2;
	_VK(vkCreateImageView(qvk.device, &img_view_info, NULL, &imv_smap_depth_array));
	ATTACH_LABEL_VARIABLE(imv_smap_depth_array, IMAGE_VIEW);

	VkCommandBuffer cmd_buf = vkpt_begin_command_buffer(&qvk.cmd_buffers_graphics);

	// C++20 VKPT: IMAGE_BARRIER
	IMAGE_BARRIER(cmd_buf,
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		//.pNext = NULL,
		.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = img_smap,
		.subresourceRange = { 
			.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
			.levelCount = 1,
			.layerCount = 2 
		},
	);

	vkpt_submit_command_buffer_simple(cmd_buf, qvk.queue_graphics, true);

	return VK_SUCCESS;
}

VkResult
vkpt_shadow_map_destroy()
{
	vkDestroyImageView(qvk.device, imv_smap_depth, NULL);
	imv_smap_depth = VK_NULL_HANDLE;
	vkDestroyImageView(qvk.device, imv_smap_depth2, NULL);
	imv_smap_depth2 = VK_NULL_HANDLE;
	vkDestroyImageView(qvk.device, imv_smap_depth_array, NULL);
	imv_smap_depth_array = VK_NULL_HANDLE;
	vkDestroyImage(qvk.device, img_smap, NULL);
	img_smap = VK_NULL_HANDLE;
	vkFreeMemory(qvk.device, mem_smap, NULL);
	mem_smap = VK_NULL_HANDLE;

	vkDestroyRenderPass(qvk.device, render_pass_smap, NULL);
	render_pass_smap = VK_NULL_HANDLE;
	vkDestroyPipelineLayout(qvk.device, pipeline_layout_smap, NULL);
	pipeline_layout_smap = VK_NULL_HANDLE;

	return VK_SUCCESS;
}

VkImageView vkpt_shadow_map_get_view()
{
	return imv_smap_depth_array;
}

VkResult
vkpt_shadow_map_create_pipelines()
{
	LOG_FUNC();

	VkPipelineShaderStageCreateInfo shader_info[] = {
		SHADER_STAGE(QVK_MOD_SHADOW_MAP_VERT, VK_SHADER_STAGE_VERTEX_BIT)
	};

	VkVertexInputBindingDescription vertex_binding_desc = {
		.binding = 0,
		.stride = sizeof(float) * 3,
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};

	VkVertexInputAttributeDescription vertex_attribute_desc = {
		.location = 0,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = 0
	};

	VkPipelineVertexInputStateCreateInfo vertex_input_info = {
		.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount   = 1,
		.pVertexBindingDescriptions      = &vertex_binding_desc,
		.vertexAttributeDescriptionCount = 1,
		.pVertexAttributeDescriptions    = &vertex_attribute_desc,
	};

	VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {
		.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	};

	VkViewport viewport = {
		.x        = 0.0f,
		.y        = 0.0f,
		.width    = (float)SHADOWMAP_SIZE,
		.height   = (float)SHADOWMAP_SIZE,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	VkRect2D scissor = {
		.offset = { 0, 0 },
		.extent = { SHADOWMAP_SIZE, SHADOWMAP_SIZE }
	};

	VkPipelineViewportStateCreateInfo viewport_state = {
		.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports    = &viewport,
		.scissorCount  = 1,
		.pScissors     = &scissor,
	};

	// C++20 VKPT: Order fix.
	VkPipelineRasterizationStateCreateInfo rasterizer_state = {
		.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.polygonMode             = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_FRONT_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.lineWidth               = 1.0f,
	};

	// C++20 VKPT: Order fix.
	VkPipelineMultisampleStateCreateInfo multisample_state = {
		.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable   = VK_FALSE,
		.minSampleShading      = 1.0f,
		.pSampleMask           = NULL,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable      = VK_FALSE,
	};

	VkPipelineDepthStencilStateCreateInfo depth_stencil_state = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS,
	};

	VkDynamicState dynamic_states[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamic_state_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = LENGTH(dynamic_states),
		.pDynamicStates = dynamic_states
	};

	VkGraphicsPipelineCreateInfo pipeline_info = {
		.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = LENGTH(shader_info),
		.pStages    = shader_info,

		.pVertexInputState   = &vertex_input_info,
		.pInputAssemblyState = &input_assembly_info,
		.pViewportState      = &viewport_state,
		.pRasterizationState = &rasterizer_state,
		.pMultisampleState   = &multisample_state,
		.pDepthStencilState  = &depth_stencil_state,
		.pColorBlendState    = NULL,
		.pDynamicState       = &dynamic_state_info,
		
		.layout              = pipeline_layout_smap,
		.renderPass          = render_pass_smap,
		.subpass             = 0,

		.basePipelineHandle  = VK_NULL_HANDLE,
		.basePipelineIndex   = -1,
	};

	_VK(vkCreateGraphicsPipelines(qvk.device, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &pipeline_smap));
	ATTACH_LABEL_VARIABLE(pipeline_smap, PIPELINE);

	VkImageView attachments[] = {
		imv_smap_depth
	};

	VkFramebufferCreateInfo fb_create_info = {
		.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass      = render_pass_smap,
		.attachmentCount = 1,
		.pAttachments    = attachments,
		.width           = SHADOWMAP_SIZE,
		.height          = SHADOWMAP_SIZE,
		.layers          = 1,
	};

	_VK(vkCreateFramebuffer(qvk.device, &fb_create_info, NULL, &framebuffer_smap));
	ATTACH_LABEL_VARIABLE(framebuffer_smap, FRAMEBUFFER);

	attachments[0] = imv_smap_depth2;
	_VK(vkCreateFramebuffer(qvk.device, &fb_create_info, NULL, &framebuffer_smap2));
	ATTACH_LABEL_VARIABLE(framebuffer_smap2, FRAMEBUFFER);

	return VK_SUCCESS;
}

VkResult
vkpt_shadow_map_destroy_pipelines()
{
	LOG_FUNC();
	vkDestroyFramebuffer(qvk.device, framebuffer_smap, NULL);
	framebuffer_smap = VK_NULL_HANDLE;
	vkDestroyFramebuffer(qvk.device, framebuffer_smap2, NULL);
	framebuffer_smap2 = VK_NULL_HANDLE;
	vkDestroyPipeline(qvk.device, pipeline_smap, NULL);
	pipeline_smap = VK_NULL_HANDLE;
	return VK_SUCCESS;
}

VkResult
vkpt_shadow_map_render(VkCommandBuffer cmd_buf, float* view_projection_matrix, int num_static_verts, int num_dynamic_verts, int transparent_offset, int num_transparent_verts)
{
	// C++20 VKPT: IMAGE_BARRIER
	IMAGE_BARRIER(cmd_buf,
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		//.pNext = NULL,
		.srcAccessMask = VK_ACCESS_SHADER_READ_BIT,
		.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = img_smap,
		.subresourceRange = { 
			.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT, 
			.levelCount = 1, 
			.layerCount = 2 
		},
	);

	VkClearValue clear_depth = { .depthStencil = { .depth = 1.f } };
	
	// C++20 VKPT: Construct fix.
	VkRenderPassBeginInfo render_pass_info = {
		.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.renderPass        = render_pass_smap,
		.framebuffer       = framebuffer_smap,
		.renderArea = {
			.offset = { 0, 0 },
			.extent = { SHADOWMAP_SIZE, SHADOWMAP_SIZE },
		},
		.clearValueCount   = 1,
		.pClearValues      = &clear_depth
	};

	vkCmdBeginRenderPass(cmd_buf, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_smap);

	VkViewport viewport =
	{
		.width = (float)SHADOWMAP_SIZE,
		.height = (float)SHADOWMAP_SIZE,
		.minDepth = 0,
		.maxDepth = 1.0f,
	};

	vkCmdSetViewport(cmd_buf, 0, 1, &viewport);

	// C++20 VKPT: Order and construct fix.
	VkRect2D scissor =
	{
		.offset = {
			.x = 0,
			.y = 0,
		},
		.extent = {
			.width = SHADOWMAP_SIZE,
			.height = SHADOWMAP_SIZE,
		},
	};

	vkCmdSetScissor(cmd_buf, 0, 1, &scissor);

	vkCmdPushConstants(cmd_buf, pipeline_layout_smap, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16, view_projection_matrix);

	VkDeviceSize vertex_offset = offsetof(struct BspVertexBuffer, positions_bsp);
	vkCmdBindVertexBuffers(cmd_buf, 0, 1, &qvk.buf_vertex_bsp.buffer, &vertex_offset);

	vkCmdDraw(cmd_buf, num_static_verts, 1, 0, 0);

	vertex_offset = offsetof(struct ModelDynamicVertexBuffer, positions_instanced);
	vkCmdBindVertexBuffers(cmd_buf, 0, 1, &qvk.buf_vertex_model_dynamic.buffer, &vertex_offset);

	vkCmdDraw(cmd_buf, num_dynamic_verts, 1, 0, 0);

	vkCmdEndRenderPass(cmd_buf);


	render_pass_info.framebuffer = framebuffer_smap2;
	vkCmdBeginRenderPass(cmd_buf, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

	vertex_offset = offsetof(struct BspVertexBuffer, positions_bsp);
	vkCmdBindVertexBuffers(cmd_buf, 0, 1, &qvk.buf_vertex_bsp.buffer, &vertex_offset);

	vkCmdDraw(cmd_buf, num_transparent_verts, 1, transparent_offset, 0);

	vkCmdEndRenderPass(cmd_buf);

	// C++20 VKPT: IMAGE_BARRIER
	IMAGE_BARRIER(cmd_buf,
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		//.pNext = NULL,
		.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
		.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = img_smap,
		.subresourceRange = { .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,.levelCount = 1,.layerCount = 2 },
	);

	return VK_SUCCESS;
}

static void sample_disk(float* u, float* v)
{
	float a = frand();
	float b = frand();

	float theta = 2.0 * M_PI * a;
	float r = std::sqrtf(b);

	*u = r * std::cosf(theta);
	*v = r * std::sinf(theta);
}

void vkpt_shadow_map_setup(const sun_light_t* light, const float* bbox_min, const float* bbox_max, float* VP, float* depth_scale, qboolean random_sampling)
{
	vec3_t up_dir = { 0.0f, 0.0f, 1.0f };
	if (light->direction[2] >= 0.99f)
		VectorSet(up_dir, 1.f, 0.f, 0.f);

	vec3_t look_dir;
	VectorScale(light->direction, -1.f, look_dir);
	VectorNormalize(look_dir);
	vec3_t left_dir;
	CrossProduct(up_dir, look_dir, left_dir);
	VectorNormalize(left_dir);
	CrossProduct(look_dir, left_dir, up_dir);
	VectorNormalize(up_dir);

	if (random_sampling)
	{
		float u, v;
		sample_disk(&u, &v);
		
		VectorMA(look_dir, u * light->angular_size_rad * 0.5f, left_dir, look_dir);
		VectorMA(look_dir, v * light->angular_size_rad * 0.5f, up_dir, look_dir);

		VectorNormalize(look_dir);

		CrossProduct(up_dir, look_dir, left_dir);
		VectorNormalize(left_dir);
		CrossProduct(look_dir, left_dir, up_dir);
		VectorNormalize(up_dir);
	}

	float view_matrix[16] = {
		left_dir[0], up_dir[0], look_dir[0], 0.f,
		left_dir[1], up_dir[1], look_dir[1], 0.f,
		left_dir[2], up_dir[2], look_dir[2], 0.f,
		0.f, 0.f, 0.f, 1.f
	};

	vec3_t view_aabb_min = { FLT_MAX, FLT_MAX, FLT_MAX };
	vec3_t view_aabb_max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (int i = 0; i < 8; i++)
	{
		float corner[4];
		corner[0] = (i & 1) ? bbox_max[0] : bbox_min[0];
		corner[1] = (i & 2) ? bbox_max[1] : bbox_min[1];
		corner[2] = (i & 4) ? bbox_max[2] : bbox_min[2];
		corner[3] = 1.f;

		float view_corner[4];
		mult_matrix_vector(view_corner, view_matrix, corner);

		view_aabb_min[0] = min(view_aabb_min[0], view_corner[0]);
		view_aabb_min[1] = min(view_aabb_min[1], view_corner[1]);
		view_aabb_min[2] = min(view_aabb_min[2], view_corner[2]);
		view_aabb_max[0] = max(view_aabb_max[0], view_corner[0]);
		view_aabb_max[1] = max(view_aabb_max[1], view_corner[1]);
		view_aabb_max[2] = max(view_aabb_max[2], view_corner[2]);
	}

	vec3_t diagonal;
	VectorSubtract(view_aabb_max, view_aabb_min, diagonal);

	float maxXY = max(diagonal[0], diagonal[1]);
	vec3_t diff;
	diff[0] = (maxXY - diagonal[0]) * 0.5f;
	diff[1] = (maxXY - diagonal[1]) * 0.5f;
	diff[2] = 0.f;
	VectorSubtract(view_aabb_min, diff, view_aabb_min);
	VectorAdd(view_aabb_max, diff, view_aabb_max);

	float projection_matrix[16];
	create_orthographic_matrix(projection_matrix, view_aabb_min[0], view_aabb_max[0], view_aabb_min[1], view_aabb_max[1], view_aabb_min[2], view_aabb_max[2]);
	mult_matrix_matrix(VP, projection_matrix, view_matrix);

	*depth_scale = view_aabb_max[2] - view_aabb_min[2];
}