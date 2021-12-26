#pragma once
#include <map>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "VkBuffer.h"
#include "VkSurface.h"
#include "VkTexture.h"
#include "Graphics/Rhi/DescriptorSet.h"

namespace Lumen::Graphics::Vulkan
{
	class VkShader;
	using setLocation = u32;
	using bindingLocation = u32;

	class VkUniform
	{
	public:
		~VkUniform();

		u32								Size{};
		VkDescriptorSetLayoutBinding	LayoutBinding{};
		std::string						Name{};
		VkShaderStageFlagBits			Stage{};
		VkUniformBuffer*				Buffer{ nullptr };
	};

	class VkSampler
	{
	public:
		VkDescriptorSetLayoutBinding LayoutBinding{};
		std::string Name{};
		VkShaderStageFlagBits Stage{};
		VkTexture* Texture{ nullptr };
	};

	class VkDescriptorSet final : public DescriptorSet
	{
	public:
		VkDescriptorSet() = default;
		explicit VkDescriptorSet(VkShader* shader, u32 setIndex);
		VkDescriptorSet(const VkDescriptorSet& right);
		~VkDescriptorSet() override { VkDescriptorSet::Release(); }

		VkDescriptorSet& operator=(const VkDescriptorSet& right);

		void Init() override;
		void Release() override;

		[[nodiscard]] constexpr u32 SetIndex() const { return mSet; }
		[[nodiscard]] constexpr ::VkDescriptorSet Set(u32 frame) const { return mSets.at(frame); }

		void Update(u32 frame) override;
		void UpdateUniform(u32 binding, const void* data, u32 frame) override;
		void SetTexture(u32 binding, Texture* texture) override;

		static void SetInterface();

	private:
		void CreateLayout();
		void CreatePool();
		void CreateDescriptorSet();

		std::map<bindingLocation, VkUniform>	mUniforms{};
		std::map<bindingLocation, VkSampler>	mSamplers{};
		VkDescriptorSetLayout					mLayout{};
		VkDescriptorPool						mPool{ nullptr };
		std::vector<::VkDescriptorSet>			mSets{ VkSurface::BufferCount };
		u32										mSet{};
		b8										mCreated{ false };
		b8										mToUpdate[VkSurface::BufferCount]{ false };

		friend class VkShader;
	};
}
