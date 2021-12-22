#include "VkPipeline.h"

#include <array>

#include "VkBuffer.h"
#include "VkDevice.h"
#include "VkContext.h"

namespace Lumen::Graphics::Vulkan
{
	/**
	 * \brief Fills the structure with the default fixed functions settings
	 * \param width Width in pixels of the target`s initial dimension
	 * \param height Height in pixels of the target`s initial dimension
	 */
	void PipelineSettings::DefaultSettings(uint32_t width, uint32_t height)
	{
		static const auto bindingDescription = VkVertexBuffer::BindingDescription();
		static const auto attributeDescriptions = VkVertexBuffer::AttributeDescriptions();

		VertexInput.vertexBindingDescriptionCount = 1;
		VertexInput.pVertexBindingDescriptions = &bindingDescription;
		VertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		VertexInput.pVertexAttributeDescriptions = attributeDescriptions.data();

		InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		InputAssembly.primitiveRestartEnable = false;

		Viewport.x = 0.f;
		Viewport.y = 0.f;
		Viewport.width = width;
		Viewport.height = height;
		Viewport.minDepth = 0.f;
		Viewport.maxDepth = 1.f;

		Scissor.offset = VkOffset2D{ 0,0 };
		Scissor.extent = VkExtent2D{ width, height };

		ViewportState.viewportCount = 1;
		ViewportState.pViewports = &Viewport;
		ViewportState.scissorCount = 1;
		ViewportState.pScissors = &Scissor;

		Rasterizer.depthClampEnable = false;
		Rasterizer.rasterizerDiscardEnable = false;
		Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		Rasterizer.lineWidth = 1.f;
		Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		Rasterizer.depthBiasEnable = false;
		Rasterizer.depthBiasConstantFactor = 0.f;
		Rasterizer.depthBiasClamp = 0.f;
		Rasterizer.depthBiasSlopeFactor = 0.f;

		Multisampling.sampleShadingEnable = false;
		Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		Multisampling.minSampleShading = 1.f;
		Multisampling.pSampleMask = nullptr;
		Multisampling.alphaToCoverageEnable = false;
		Multisampling.alphaToOneEnable = false;

		ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		ColorBlendAttachment.blendEnable = false;
		ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		ColorBlending.logicOpEnable = false;
		ColorBlending.logicOp = VK_LOGIC_OP_COPY;
		ColorBlending.attachmentCount = 1;
		ColorBlending.pAttachments = &ColorBlendAttachment;
		ColorBlending.blendConstants[0] = 0.f;
		ColorBlending.blendConstants[1] = 0.f;
		ColorBlending.blendConstants[2] = 0.f;
		ColorBlending.blendConstants[3] = 0.f;

		DynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		DynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

		DynamicState.dynamicStateCount = static_cast<uint32_t>(DynamicStates.size());
		DynamicState.pDynamicStates = DynamicStates.data();
	}

	VkPipeline::VkPipeline(const VkShader& shader, const PipelineSettings& settings, VkRenderPass* renderPass)
		: mRenderPass{ renderPass }
	{
		CreatePipeline(shader, settings);
	}

	/**
	 * \brief Sets the current pipeline as the one to be used from this command on
	 * \param cmd Buffer used to memorize given commands
	 */
	void VkPipeline::Bind(VkCommandBuffer cmd)
	{
		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

		const VkViewport vp
		{
			0, 0,
			static_cast<float>(mRenderPass->Width()), static_cast<float>(mRenderPass->Height()),
			0, 1
		};
		const VkRect2D scissor
		{
			VkOffset2D{ 0, 0 },
			VkExtent2D{ mRenderPass->Width(), mRenderPass->Height() }
		};

		vkCmdSetViewport(cmd, 0, 1, &vp);
		vkCmdSetScissor(cmd, 0, 1, &scissor);
	}

	void VkPipeline::BindDescriptorSet(const VkDescriptorSet& set, VkCommandBuffer cmd, uint32_t frame)
	{
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mLayout, set.Set, 1, &set.Sets[frame], 0, nullptr);
	}

	/**
	 * \brief Destroys all the class` resources
	 */
	void VkPipeline::Release()
	{
		vkDestroyPipeline(VkContext::Get().LogicalDevice(), mPipeline ,nullptr);
		vkDestroyPipelineLayout(VkContext::Get().LogicalDevice(), mLayout, nullptr);
	}

	/**
	 * \brief Creates a new graphics pipeline
	 * \param shader Collection of shader programs used in the pipeline
	 * \param settings Pipeline`s fixed functions settings
	 */
	void VkPipeline::CreatePipeline(const VkShader& shader, const PipelineSettings& settings)
	{
		{
			auto descriptorLayout{ shader.DescriptorsLayout() };
			VkPipelineLayoutCreateInfo createInfo{};
			createInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			createInfo.setLayoutCount			= static_cast<uint32_t>(descriptorLayout.size());
			createInfo.pSetLayouts				= descriptorLayout.data();
			createInfo.pushConstantRangeCount	= 0;
			createInfo.pPushConstantRanges		= nullptr;

			VK_ASSERT(vkCreatePipelineLayout(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mLayout), "Failed to create pipeline layout");
		}

		VkGraphicsPipelineCreateInfo createInfo{};
		createInfo.stageCount			= static_cast<uint32_t>(shader.GetPipelineStages().size());
		createInfo.pStages				= shader.GetPipelineStages().data();
		createInfo.pVertexInputState	= &settings.VertexInput;
		createInfo.pInputAssemblyState	= &settings.InputAssembly;
		createInfo.pViewportState		= &settings.ViewportState;
		createInfo.pRasterizationState	= &settings.Rasterizer;
		createInfo.pMultisampleState	= &settings.Multisampling;
		createInfo.pDepthStencilState	= nullptr;
		createInfo.pColorBlendState		= &settings.ColorBlending;
		createInfo.pDynamicState		= &settings.DynamicState;
		createInfo.layout				= mLayout;
		createInfo.renderPass			= mRenderPass->Get();
		createInfo.subpass				= 0;
		createInfo.basePipelineHandle	= nullptr;
		createInfo.basePipelineIndex	= -1;

		vkCreateGraphicsPipelines(VkContext::Get().LogicalDevice(), nullptr, 1, &createInfo, nullptr, &mPipeline);
	}
}
