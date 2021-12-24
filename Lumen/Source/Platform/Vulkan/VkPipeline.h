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

		void DefaultSettings(uint32_t width, uint32_t height);
	};

	class VkPipeline : public Pipeline
	{
	public:
		VkPipeline() = default;
		explicit VkPipeline(const VkShader& shader, const PipelineSettings& settings, VkRenderPass* renderPass);
		~VkPipeline() override { VkPipeline::Release(); }

		void Init() override;
		void Release() override;

		void Bind() override;
		void BindDescriptorSet(const DescriptorSet& set, uint32_t frame) override;

		static void SetInterface();

	private:
		void CreatePipeline(const VkShader& shader, const PipelineSettings& settings);

		VkRenderPass*		mRenderPass{ nullptr };
		VkPipelineLayout	mLayout{};
		::VkPipeline		mPipeline{};
	};
}
