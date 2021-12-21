#pragma once
#include <vector>
#include <vulkan/vulkan.h>

#include "Core/Window.h"
#include "Graphics/Rhi/GraphicsSurface.h"

namespace Lumen::Graphics::Vulkan
{
	class VkRenderer;

	class VkSurface : public GraphicsSurface
	{
	public:
		constexpr static uint32_t BufferCount{ 3 };
		
		VkSurface() = default;
		explicit VkSurface(Window& window);
		~VkSurface() { Release(); }

		[[nodiscard]] constexpr VkFormat Format() const { return mFormat.format; }
		[[nodiscard]] constexpr uint32_t Width() const { return mExtent.width; }
		[[nodiscard]] constexpr uint32_t Height() const { return mExtent.height; }
		[[nodiscard]] constexpr VkImageView ImageView(uint32_t frame) const { return mImageViews[frame]; }
		[[nodiscard]] constexpr VkSurfaceKHR Get() const { return mSurface; }
		[[nodiscard]] constexpr VkCommandBuffer CommandBuffer() const { return mCommandBuffers[mImageIndex]; }
		[[nodiscard]] constexpr uint32_t ImageIndex() const { return mImageIndex; }

		void Initialize();
		void Release();

		void BeginFrame();
		void EndFrame();

	private:
		void QuerySupport();
		void ChooseSettings();
		void ChooseExtent();
		void CreateSwapchain();
		void CreateImageViews();
		void CreateCommandBuffers();
		void CreateSyncObjects();
		void Resize();

		uint32_t						mCurrentFrame{ 0 };
		uint32_t						mImageIndex{ 0 };
		VkCommandPool					mCommandPool{};
		std::vector<VkSemaphore>		mImageAvailableSemaphores{};
		std::vector<VkSemaphore>		mRenderFinishedSemaphores{};
		std::vector<VkFence>			mInFlightFences;
		std::vector<VkCommandBuffer>	mCommandBuffers{};
		std::vector<VkImage>			mImages{};
		std::vector<VkImageView>		mImageViews{};
		bool							mInitialized{ false };
		VkSwapchainKHR					mSwapchain{};
		VkSurfaceKHR					mSurface{};
		VkSurfaceFormatKHR				mFormat{};
		VkPresentModeKHR				mPresentMode{};
		VkExtent2D						mExtent{};

		constexpr static uint32_t		MaxFramesInFlight{ BufferCount - 1 };
	};
}
