#pragma once
#include <vulkan/vulkan.h>

#include "VkShader.h"
#include "VkRenderPass.h"

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

	class VkPipeline
	{
	public:
		VkPipeline() = default;
		explicit VkPipeline(const VkShader& shader, const PipelineSettings& settings, VkRenderPass* renderPass);
		~VkPipeline() { Release(); }

		void Bind(VkCommandBuffer cmd);
		void BindDescriptorSet(const VkDescriptorSet& set, VkCommandBuffer cmd, uint32_t frame);
		void Release();

	private:
		void CreatePipeline(const VkShader& shader, const PipelineSettings& settings);

		VkRenderPass*		mRenderPass{ nullptr };
		VkPipelineLayout	mLayout{};
		::VkPipeline		mPipeline{};
	};
}
