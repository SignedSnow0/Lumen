#include "VkPipeline.h"

#include <array>

#include "VkBuffer.h"
#include "VkDevice.h"
#include "VkContext.h"

namespace Lumen::Graphics::Vulkan
{
	namespace
	{
		Pipeline* CreateFuncVulkan(const PipelineInitInfo& initInfo)
		{
			const VkShader& shader = *dynamic_cast<VkShader*>(initInfo.Shader);
			const auto renderPass = dynamic_cast<VkRenderPass*>(initInfo.RenderPass);
			PipelineSettings settings{};
			settings.DefaultSettings(initInfo.InitialWidth, initInfo.InitialHeight);
			switch (initInfo.CullMode)
			{
			case CullMode::Front:
				settings.Rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
				break;
			case CullMode::Back: 
				settings.Rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
				break;
			case CullMode::FrontAndBack:
				settings.Rasterizer.cullMode = VK_CULL_MODE_FRONT_AND_BACK;
				break;
			case CullMode::None: 
				settings.Rasterizer.cullMode = VK_CULL_MODE_NONE;
				break;
			}
			switch (initInfo.DrawType)
			{
			case DrawType::Point:
				settings.InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
				break;
			case DrawType::Triangle: 
				settings.InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
				break;
			case DrawType::Line: 
				settings.InputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
				break;
			}
			switch (initInfo.PolygonMode)
			{
			case PolygonMode::Fill:
				settings.Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
				break;
			case PolygonMode::Line:
				settings.Rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
				break;
			case PolygonMode::Point:
				settings.Rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
				break;
			}
			//todo blendmode

			return new VkPipeline{ &shader, settings, renderPass };
		}
	}

	/**
	 * @brief Fills the structure with the default fixed functions settings
	 * @param width Width in pixels of the target`s initial dimension
	 * @param height Height in pixels of the target`s initial dimension
	 */
	void PipelineSettings::DefaultSettings(u32 width, u32 height)
	{
		static const auto bindingDescription = VkVertexBuffer::BindingDescription();
		static const auto attributeDescriptions = VkVertexBuffer::AttributeDescriptions();

		VertexInput.sType							= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		VertexInput.vertexBindingDescriptionCount	= 1;
		VertexInput.pVertexBindingDescriptions		= &bindingDescription;
		VertexInput.vertexAttributeDescriptionCount = static_cast<u32>(attributeDescriptions.size());
		VertexInput.pVertexAttributeDescriptions	= attributeDescriptions.data();

		InputAssembly.sType						= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssembly.topology					= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		InputAssembly.primitiveRestartEnable	= false;

		Viewport.x			= 0.f;
		Viewport.y			= 0.f;
		Viewport.width		= static_cast<f32>(width);
		Viewport.height		= static_cast<f32>(height);
		Viewport.minDepth	= 0.f;
		Viewport.maxDepth	= 1.f;

		Scissor.offset = VkOffset2D{ 0,0 };
		Scissor.extent = VkExtent2D{ width, height };

		ViewportState.sType			= VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		ViewportState.viewportCount = 1;
		ViewportState.pViewports	= &Viewport;
		ViewportState.scissorCount	= 1;
		ViewportState.pScissors		= &Scissor;

		Rasterizer.sType					= VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		Rasterizer.depthClampEnable			= false;
		Rasterizer.rasterizerDiscardEnable	= false;
		Rasterizer.polygonMode				= VK_POLYGON_MODE_FILL;
		Rasterizer.lineWidth				= 1.f;
		Rasterizer.cullMode					= VK_CULL_MODE_BACK_BIT;
		Rasterizer.frontFace				= VK_FRONT_FACE_COUNTER_CLOCKWISE;
		Rasterizer.depthBiasEnable			= false;
		Rasterizer.depthBiasConstantFactor	= 0.f;
		Rasterizer.depthBiasClamp			= 0.f;
		Rasterizer.depthBiasSlopeFactor		= 0.f;

		Multisampling.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		Multisampling.sampleShadingEnable	= false;
		Multisampling.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;
		Multisampling.minSampleShading		= 1.f;
		Multisampling.pSampleMask			= nullptr;
		Multisampling.alphaToCoverageEnable = false;
		Multisampling.alphaToOneEnable		= false;

		ColorBlendAttachment.colorWriteMask			= VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		ColorBlendAttachment.blendEnable			= false;
		ColorBlendAttachment.srcColorBlendFactor	= VK_BLEND_FACTOR_ONE;
		ColorBlendAttachment.dstColorBlendFactor	= VK_BLEND_FACTOR_ZERO;
		ColorBlendAttachment.colorBlendOp			= VK_BLEND_OP_ADD;
		ColorBlendAttachment.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ONE;
		ColorBlendAttachment.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO;
		ColorBlendAttachment.alphaBlendOp			= VK_BLEND_OP_ADD;

		ColorBlending.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		ColorBlending.logicOpEnable		= false;
		ColorBlending.logicOp			= VK_LOGIC_OP_COPY;
		ColorBlending.attachmentCount	= 1;
		ColorBlending.pAttachments		= &ColorBlendAttachment;
		ColorBlending.blendConstants[0] = 0.f;
		ColorBlending.blendConstants[1] = 0.f;
		ColorBlending.blendConstants[2] = 0.f;
		ColorBlending.blendConstants[3] = 0.f;

		DynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		DynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);

		DynamicState.sType				= VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		DynamicState.dynamicStateCount	= static_cast<u32>(DynamicStates.size());
		DynamicState.pDynamicStates		= DynamicStates.data();

		DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		DepthStencil.depthTestEnable = VK_TRUE;
		DepthStencil.depthWriteEnable = VK_TRUE;
		DepthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		DepthStencil.depthBoundsTestEnable = VK_FALSE;
		DepthStencil.minDepthBounds = 0.0f;
		DepthStencil.maxDepthBounds = 1.0f;
		DepthStencil.stencilTestEnable = VK_FALSE;
		DepthStencil.front = {};
		DepthStencil.back = {};
	}

	VkPipeline::VkPipeline(const VkShader* shader, const PipelineSettings& settings, VkRenderPass* renderPass)
		: mShader{shader}, mRenderPass{renderPass}
	{
		CreatePipeline(*shader, settings);
	}

	/**
	 * @brief Sets the current pipeline as the one to be used from this command on
	 */
	void VkPipeline::Bind()
	{
		VkCommandBuffer cmd = VkSurface::Bound()->CommandBuffer();

		vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

		const VkViewport vp
		{
			0, 0,
			static_cast<f32>(mRenderPass->Width()), static_cast<f32>(mRenderPass->Height()),
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

	void VkPipeline::BindDescriptorSet(const DescriptorSet& set, u32 frame)
	{
		VkCommandBuffer cmd = VkSurface::Bound()->CommandBuffer();

		const auto vkSet = reinterpret_cast<const VkDescriptorSet*>(&set);
		const ::VkDescriptorSet dSet = vkSet->Set(frame);
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mLayout, vkSet->SetIndex(), 1, &dSet, 0, nullptr);
	}

	void VkPipeline::SetPushConstant(u32 index, const void* data)
	{
		if (index < mShader->PushConstants().size())
		{
			VkCommandBuffer cmd = VkSurface::Bound()->CommandBuffer();
			
			const auto& [stageFlags, offset, size] = mShader->PushConstants()[index];
			vkCmdPushConstants(cmd, mLayout, stageFlags, offset, size, data);
		}
	}

	void VkPipeline::SetInterface()
	{
		sCreateFunc = CreateFuncVulkan;
	}

	void VkPipeline::Init()
	{

	}

	/**
	 * @brief Destroys all the class` resources
	 */
	void VkPipeline::Release()
	{
		vkDestroyPipeline(VkContext::Get().LogicalDevice(), mPipeline ,nullptr);
		vkDestroyPipelineLayout(VkContext::Get().LogicalDevice(), mLayout, nullptr);
	}

	/**
	 * @brief Creates a new graphics pipeline
	 * @param shader Collection of shader programs used in the pipeline
	 * @param settings Pipeline`s fixed functions settings
	 */
	void VkPipeline::CreatePipeline(const VkShader& shader, const PipelineSettings& settings)
	{
		{
			const auto descriptorLayouts{ shader.DescriptorsLayout() };
			const auto pushConstants{ shader.PushConstants() };
			
			VkPipelineLayoutCreateInfo createInfo{};
			createInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			createInfo.setLayoutCount			= static_cast<u32>(descriptorLayouts.size());
			createInfo.pSetLayouts				= descriptorLayouts.data();
			createInfo.pushConstantRangeCount	= static_cast<u32>(pushConstants.size());
			createInfo.pPushConstantRanges		= pushConstants.data();

			VK_ASSERT(vkCreatePipelineLayout(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mLayout), "Failed to create pipeline layout");

			for (auto& layout : descriptorLayouts)//destroys layouts created from shader.DescriptorsLayout()
				vkDestroyDescriptorSetLayout(VkContext::Get().LogicalDevice(), layout, nullptr);
		}

		VkGraphicsPipelineCreateInfo createInfo{};
		createInfo.sType				= VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.stageCount			= static_cast<u32>(shader.PipelineStages().size());
		createInfo.pStages				= shader.PipelineStages().data();
		createInfo.pVertexInputState	= &settings.VertexInput;
		createInfo.pInputAssemblyState	= &settings.InputAssembly;
		createInfo.pViewportState		= &settings.ViewportState;
		createInfo.pRasterizationState	= &settings.Rasterizer;
		createInfo.pMultisampleState	= &settings.Multisampling;
		createInfo.pDepthStencilState	= &settings.DepthStencil;
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
