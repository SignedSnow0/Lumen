#pragma once
#include <filesystem>
#include <vma/vk_mem_alloc.h>

#include <vulkan/vulkan.h>

#include "Core/Types.h"
#include "Graphics/Rhi/Texture.h"

namespace Lumen::Graphics::Vulkan
{
	class VkBuffer;

	class VkTexture : public Texture
	{
	public:
		explicit VkTexture(std::filesystem::path source);
		~VkTexture() override { VkTexture::Release(); }

		[[nodiscard]] constexpr ::VkSampler Sampler() const { return mSampler; }
		[[nodiscard]] constexpr VkImageView View() const { return mView; }
		[[nodiscard]] constexpr VkImageLayout Layout() const { return mLayout; }

		void Release() override;

		static void SetInterface();

	private:
		void LoadImage();
		void CreateImage(const VkBuffer& buffer, u32 width, u32 height);
		void CreateView();
		void CreateSampler();

		std::filesystem::path	mSource{};
		VkImage					mImage{};
		VkImageView				mView{};
		::VkSampler				mSampler{};
		VkImageLayout			mLayout{};
		VmaAllocation			mAllocation{};
		u32						mWidth{};
		u32						mHeight{};
		u32						mMipLevels{};
	};
}
