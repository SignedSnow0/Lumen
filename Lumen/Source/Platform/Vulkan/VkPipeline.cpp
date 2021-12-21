#include "VkPipeline.h"

#include "VkBuffer.h"
#include "VkDevice.h"

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

		InputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
		InputAssembly.primitiveRestartEnable = false;

		Viewport.x = 0.f;
		Viewport.y = 0.f;
		Viewport.width = width;
		Viewport.height = height;
		Viewport.minDepth = 0.f;
		Viewport.maxDepth = 1.f;

		Scissor.offset = vk::Offset2D{ 0,0 };
		Scissor.extent = vk::Extent2D{ width, height };

		ViewportState.viewportCount = 1;
		ViewportState.pViewports = &Viewport;
		ViewportState.scissorCount = 1;
		ViewportState.pScissors = &Scissor;

		Rasterizer.depthClampEnable = false;
		Rasterizer.rasterizerDiscardEnable = false;
		Rasterizer.polygonMode = vk::PolygonMode::eFill;
		Rasterizer.lineWidth = 1.f;
		Rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		Rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
		Rasterizer.depthBiasEnable = false;
		Rasterizer.depthBiasConstantFactor = 0.f;
		Rasterizer.depthBiasClamp = 0.f;
		Rasterizer.depthBiasSlopeFactor = 0.f;

		Multisampling.sampleShadingEnable = false;
		Multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
		Multisampling.minSampleShading = 1.f;
		Multisampling.pSampleMask = nullptr;
		Multisampling.alphaToCoverageEnable = false;
		Multisampling.alphaToOneEnable = false;

		ColorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR| vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		ColorBlendAttachment.blendEnable = false;
		ColorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne;
		ColorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero;
		ColorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd;
		ColorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne;
		ColorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero;
		ColorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd;

		ColorBlending.logicOpEnable = false;
		ColorBlending.logicOp = vk::LogicOp::eCopy;
		ColorBlending.attachmentCount = 1;
		ColorBlending.pAttachments = &ColorBlendAttachment;
		ColorBlending.blendConstants[0] = 0.f;
		ColorBlending.blendConstants[1] = 0.f;
		ColorBlending.blendConstants[2] = 0.f;
		ColorBlending.blendConstants[3] = 0.f;

		DynamicStates.push_back(vk::DynamicState::eViewport);
		DynamicStates.push_back(vk::DynamicState::eScissor);

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
	void VkPipeline::Bind(vk::CommandBuffer cmd)
	{
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);

		const vk::Viewport vp
		{
			0, 0,
			static_cast<float>(mRenderPass->Width()), static_cast<float>(mRenderPass->Height()),
			0, 1
		};
		const vk::Rect2D scissor
		{
			vk::Offset2D{ 0, 0 },
			vk::Extent2D{ mRenderPass->Width(), mRenderPass->Height() }
		};

		cmd.setViewport(0, 1, &vp);
		cmd.setScissor(0, 1, &scissor);
	}

	void VkPipeline::BindDescriptorSet(const VkDescriptorSet& set, vk::CommandBuffer cmd, uint32_t frame)
	{
		cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, mLayout, set.Set, 1, &set.Sets[frame], 0, nullptr);
	}

	/**
	 * \brief Destroys all the class` resources
	 */
	void VkPipeline::Release()
	{
		VkGraphics::Device().Device().destroyPipeline(mPipeline);
		VkGraphics::Device().Device().destroyPipelineLayout(mLayout);
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
			vk::PipelineLayoutCreateInfo createInfo{};
			createInfo.setLayoutCount			= static_cast<uint32_t>(descriptorLayout.size());
			createInfo.pSetLayouts				= descriptorLayout.data();
			createInfo.pushConstantRangeCount	= 0;
			createInfo.pPushConstantRanges		= nullptr;

			VK_ASSERT(VkGraphics::Device().Device().createPipelineLayout(&createInfo, nullptr, &mLayout), "Failed to create pipeline layout");
		}

		vk::GraphicsPipelineCreateInfo createInfo{};
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

		mPipeline = VkGraphics::Device().Device().createGraphicsPipeline(nullptr, createInfo).value;
	}
}
