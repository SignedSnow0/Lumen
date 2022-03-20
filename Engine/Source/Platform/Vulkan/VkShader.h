#pragma once
#include <filesystem>
#include <map>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "VkDescriptorSet.h"
#include "Graphics/Rhi/Shader.h"

namespace Lumen::Graphics::Vulkan
{
	class VkUniformBuffer;
	class VkPipeline;

	class VkShader final : public Shader
	{
	public:
		VkShader() = default;
		explicit VkShader(const std::unordered_map<std::string, ShaderStage>& sources);
		~VkShader() override { VkShader::Release(); }

		[[nodiscard]] const std::vector<VkPipelineShaderStageCreateInfo>& PipelineStages() const	{ return mPipelineInfos; }
		[[nodiscard]] const VkDescriptorSet& DescriptorSet(u32 setIndex) const						{ return mDescriptorSets.at(setIndex); }
		[[nodiscard]] std::vector<VkDescriptorSetLayout> DescriptorsLayout() const;
		[[nodiscard]] const std::vector<VkPushConstantRange>& PushConstants() const { return mPushConstants; }
		
		void Init() override;
		void Release() override;

		static void SetInterface();

	private:
		void CreateModule(const std::vector<u32>& blob, VkShaderStageFlagBits stage);
		void Reflect(const std::vector<u32>& blob, VkShaderStageFlagBits stage);

		std::vector<VkShaderModule>						mModules{};
		std::vector<VkPipelineShaderStageCreateInfo>	mPipelineInfos{};
		
		std::map<setLocation, VkDescriptorSet>			mDescriptorSets{};
		std::vector<VkPushConstantRange>				mPushConstants{};
	};
}
