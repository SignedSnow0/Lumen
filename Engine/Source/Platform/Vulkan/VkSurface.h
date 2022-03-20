#pragma once
#include <vector>
#include <vulkan/vulkan.h>

#include "Core/Window.h"
#include "Graphics/Rhi/Surface.h"

namespace Lumen::Graphics::Vulkan
{
	class VkRenderer;

	class VkSurface final : public Surface
	{
	public:
		constexpr static u32 BufferCount{ 3 };
		
		VkSurface() = default;
		explicit VkSurface(Window* window);
		~VkSurface() override { VkSurface::Release(); }

		[[nodiscard]] constexpr VkFormat Format() const					{ return mFormat.format; }
		[[nodiscard]] constexpr u32 Width() const						{ return mExtent.width; }
		[[nodiscard]] constexpr u32 Height() const						{ return mExtent.height; }
		[[nodiscard]] constexpr VkImageView ImageView(u32 frame) const	{ return mFrames[frame].ImageView; }
		[[nodiscard]] constexpr VkSurfaceKHR Get() const				{ return mSurface; }
		[[nodiscard]] constexpr VkCommandBuffer CommandBuffer() const	{ return mFrames[mImageIndex].CommandBuffer; }
		[[nodiscard]] constexpr u32 CurrentFrame() const override		{ return mImageIndex; }

		[[nodiscard]] static VkSurface* Bound() { return sBound; }

		void Init() override;
		void Release() override;

		void Begin() override;
		void End() override;

		static void SetInterface();

	private:
		void QuerySupport();
		void ChooseSettings();
		void ChooseExtent();
		void CreateSwapchain();
		void CreateImageViews();
		void CreateCommandBuffers();
		void CreateSyncObjects();
		void Resize();

		struct FrameData
		{
			VkSemaphore		ImageAvailable{};
			VkSemaphore		RenderFinished{};
			VkFence			InFlight{};
			VkCommandBuffer CommandBuffer{};
			VkImage			Image{};
			VkImageView		ImageView{};
		};

		std::vector<FrameData>	mFrames{ BufferCount };
		u32						mCurrentFrame{ 0 };
		u32						mImageIndex{ 0 };
		b8						mInitialized{ false };
		VkCommandPool			mCommandPool{};
		VkSwapchainKHR			mSwapchain{};
		VkSurfaceKHR			mSurface{};
		VkSurfaceFormatKHR		mFormat{};
		VkPresentModeKHR		mPresentMode{};
		VkExtent2D				mExtent{};
		Window*					mWindow{ nullptr };

		constexpr static u32	MaxFramesInFlight{ BufferCount - 1 };
		static VkSurface*		sBound;
	};
}
