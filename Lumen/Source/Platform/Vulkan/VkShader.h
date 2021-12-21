#pragma once
#include <filesystem>
#include <map>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "VkSurface.h"

namespace Lumen::Graphics::Vulkan
{
	class VkUniformBuffer;

	using setLocation = uint32_t;
	using bindingLocation = uint32_t;

	struct VkUniform
	{
		VkDescriptorSetLayoutBinding	LayoutBinding{};
		std::string						Name{};
		VkShaderStageFlagBits			Stage{};
	};

	struct VkDescriptorSet
	{
		VkDescriptorSet() = default;
		VkDescriptorSet(const VkDescriptorSet& right);
		~VkDescriptorSet() = default;

		VkDescriptorSet& operator=(const VkDescriptorSet& right);
		
		std::map<bindingLocation, VkUniform>		Uniforms{};
		VkDescriptorSetLayout						Layout{};
		VkDescriptorPool							Pool{};
		std::vector<VkDescriptorSet>				Sets{ VkSurface::BufferCount };
		uint32_t									Set{};

		void Initialize();
		void Release();

		void AttachUniformBuffer(const VkUniformBuffer& buffer, uint32_t binding);
	};

	class VkShader
	{
	public:
		VkShader() = default;
		explicit VkShader(const std::unordered_map<std::string, VkShaderStageFlagBits>& sources);
		~VkShader() { Release(); }

		[[nodiscard]] const std::vector<VkPipelineShaderStageCreateInfo>& GetPipelineStages() const { return mPipelineInfos; }
		[[nodiscard]] const VkDescriptorSet& GetDescriptorSet(uint32_t setIndex) const { return mDescriptorSets.at(setIndex); }
		[[nodiscard]] std::vector<VkDescriptorSetLayout> DescriptorsLayout() const;

		void Release();

	private:
		void CreateModule(const std::vector<uint32_t>& blob, VkShaderStageFlagBits stage);
		void Reflect(const std::vector<uint32_t>& blob, VkShaderStageFlagBits stage);

		std::vector<VkShaderModule>						mModules{};
		std::vector<VkPipelineShaderStageCreateInfo>	mPipelineInfos{};

		std::map<setLocation, VkDescriptorSet>			mDescriptorSets{};
	};

	class EngineShaders
	{
	public:
		enum Shaders : uint32_t
		{
			Default,

			Count
		};

		EngineShaders() = delete;

		constexpr static VkShader& GetShader(Shaders shader) { return *sShaders[shader]; }

		static void Initialize();
		static void Shutdown();

	private:
		static std::vector<VkShader*> sShaders;
	};
}
