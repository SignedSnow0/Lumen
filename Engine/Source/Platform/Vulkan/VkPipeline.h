#pragma once
#include <vulkan/vulkan.h>

#include "VkShader.h"
#include "VkRenderPass.h"
#include "Graphics/Rhi/Pipeline.h"

namespace Lumen::Graphics::Vulkan
{
	struct PipelineSettings
	{
		VkPipelineVertexInputStateCreateInfo	VertexInput{};
		VkPipelineInputAssemblyStateCreateInfo	InputAssembly{};
		VkViewport								Viewport{};
		VkRect2D								Scissor{};
		VkPipelineViewportStateCreateInfo		ViewportState{};
		VkPipelineRasterizationStateCreateInfo	Rasterizer{};
		VkPipelineMultisampleStateCreateInfo	Multisampling{};
		VkPipelineColorBlendAttachmentState		ColorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo		ColorBlending{};
		std::vector<VkDynamicState>				DynamicStates{};
		VkPipelineDynamicStateCreateInfo		DynamicState{};
		VkPipelineDepthStencilStateCreateInfo	DepthStencil{};
		
		void DefaultSettings(u32 width, u32 height);
	};

	class VkPipeline : public Pipeline
	{
	public:
		VkPipeline() = default;
		explicit VkPipeline(const VkShader* shader, const PipelineSettings& settings, VkRenderPass* renderPass);
		~VkPipeline() override { VkPipeline::Release(); }

		void Init() override;
		void Release() override;

		void Bind() override;
		void BindDescriptorSet(const DescriptorSet& set, u32 frame) override;
		void SetPushConstant(u32 index, const void* data) override;
		
		static void SetInterface();

	private:
		void CreatePipeline(const VkShader& shader, const PipelineSettings& settings);

		const VkShader*		mShader{ nullptr };
		VkRenderPass*		mRenderPass{ nullptr };
		VkPipelineLayout	mLayout{};
		::VkPipeline		mPipeline{};
	};
}
