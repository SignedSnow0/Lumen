#pragma once
#include <filesystem>
#include <vma/vk_mem_alloc.h>

#include <vulkan/vulkan.h>

#include "Core/Types.h"
#include "Graphics/Rhi/Texture.h"

namespace Lumen::Graphics::Vulkan
{
	class VkBuffer;

	enum class TextureUsage : u32
	{
		RenderTarget = 0,
		DepthTarget,
		Image,
		
		Count
	};
	
	class VkTexture : public Texture
	{
	public:
		explicit VkTexture(u32 width, u32 height, TextureUsage usage);
		explicit VkTexture(std::filesystem::path source);
		~VkTexture() override { VkTexture::Release(); }

		[[nodiscard]] constexpr u32 Width() const { return mWidth; }
		[[nodiscard]] constexpr u32 Height() const { return mHeight; }
		[[nodiscard]] constexpr ::VkSampler Sampler() const { return mSampler; }
		[[nodiscard]] constexpr VkImageView View() const { return mView; }
		[[nodiscard]] constexpr VkImageLayout Layout() const { return mLayout; }

		void Release() override;

		void Recreate(u32 width, u32 height);

		static void SetInterface();

	private:
		void LoadImage();
		void CreateImage(const VkBuffer& buffer, u32 width, u32 height);
		void CreateEmptyImage(u32 width, u32 height);
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
		u32						mMipLevels{ 1 };
		TextureUsage			mUsage{ TextureUsage::Image };
	};
}
