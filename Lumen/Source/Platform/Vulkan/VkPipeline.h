#pragma once
#include <vulkan/vulkan.hpp>

#include "VkShader.h"
#include "VkRenderPass.h"

namespace FrostEngine::Vulkan
{
	struct PipelineSettings
	{
		vk::PipelineVertexInputStateCreateInfo		VertexInput{};
		vk::PipelineInputAssemblyStateCreateInfo	InputAssembly{};
		vk::Viewport								Viewport{};
		vk::Rect2D									Scissor{};
		vk::PipelineViewportStateCreateInfo			ViewportState{};
		vk::PipelineRasterizationStateCreateInfo	Rasterizer{};
		vk::PipelineMultisampleStateCreateInfo		Multisampling{};
		vk::PipelineColorBlendAttachmentState		ColorBlendAttachment{};
		vk::PipelineColorBlendStateCreateInfo		ColorBlending{};
		std::vector<vk::DynamicState>				DynamicStates{};
		vk::PipelineDynamicStateCreateInfo			DynamicState{};

		void DefaultSettings(uint32_t width, uint32_t height);
	};

	class VkPipeline
	{
	public:
		VkPipeline() = default;
		explicit VkPipeline(const VkShader& shader, const PipelineSettings& settings, VkRenderPass* renderPass);
		~VkPipeline() { Release(); }

		void Bind(vk::CommandBuffer cmd);
		void BindDescriptorSet(const VkDescriptorSet& set, vk::CommandBuffer cmd, uint32_t frame);
		void Release();

	private:
		void CreatePipeline(const VkShader& shader, const PipelineSettings& settings);

		VkRenderPass*		mRenderPass{ nullptr };
		vk::PipelineLayout	mLayout{};
		vk::Pipeline		mPipeline{};
	};
}
