#include "VkSurface.h"

#include <algorithm>
#include <cassert>

#include "VkContext.h"
#include "VkDevice.h"
#ifdef _WIN64
#include <GLFW/glfw3.h>
#endif

namespace Lumen::Graphics::Vulkan
{
	namespace
	{
		struct
		{
			VkSurfaceCapabilitiesKHR		Capabilities{};
			std::vector<VkSurfaceFormatKHR> Formats{};
			std::vector<VkPresentModeKHR>	PresentModes{};
		} capabilities;

		Surface* CreateFuncVulkan(Window* window)
		{
			return new VkSurface{ window };
		}
	}

	VkSurface* VkSurface::sBound{ nullptr };

	VkSurface::VkSurface(Window* window)
		: mWindow{ window }
	{
	}

	/**
	 * @brief Creates all the necessary resources for the class
	 */
	void VkSurface::Init()
	{
		assert(mWindow);
		VkResult result = glfwCreateWindowSurface(VkContext::Get().Instance(), mWindow->Native(), nullptr, &mSurface);
		VkBool32 supported;
		VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(VkContext::Get().Device().PhysicalDevice(), 0, mSurface, &supported), "Failed to query surface support");

		ChooseSettings();
		ChooseExtent();
		CreateSwapchain();
		CreateImageViews();

		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType			= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.queueFamilyIndex = VkContext::Get().Device().GraphicsFamily();
		createInfo.flags			= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VK_ASSERT(vkCreateCommandPool(VkContext::Get().Device().Device(), &createInfo, nullptr, &mCommandPool), "Failed to create command pool")

		CreateCommandBuffers();
		CreateSyncObjects();

		mInitialized = true;
	}

	/**
	 * @brief Destroys all the class` resources
	 */
	void VkSurface::Release()
	{
		if (mInitialized)
		{
			for (u32 i{ 0 }; i < BufferCount; i++)
			{
				vkDestroySemaphore(VkContext::Get().LogicalDevice(), mFrames[i].ImageAvailable, nullptr);
				vkDestroySemaphore(VkContext::Get().LogicalDevice(), mFrames[i].RenderFinished, nullptr);
				vkDestroyFence(VkContext::Get().LogicalDevice(), mFrames[i].InFlight, nullptr);
			}

			vkDestroyCommandPool(VkContext::Get().LogicalDevice(), mCommandPool, nullptr);

			vkDestroySwapchainKHR(VkContext::Get().LogicalDevice(), mSwapchain, nullptr);
			for (const auto& frame : mFrames)
				vkDestroyImageView(VkContext::Get().LogicalDevice(), frame.ImageView, nullptr);

			vkDestroySurfaceKHR(VkContext::Get().Instance(), mSurface, nullptr);
		}
	}

	/**
	 * @brief Retrives the next available frame and begins command buffer recording
	 */
	void VkSurface::Begin()
	{
		VK_ASSERT(vkWaitForFences(VkContext::Get().LogicalDevice(), 1, &mFrames[mCurrentFrame].InFlight, true, UINT64_MAX), "Failed to wait for fence")

		const VkResult result = vkAcquireNextImageKHR(VkContext::Get().LogicalDevice(), mSwapchain, UINT64_MAX, mFrames[mCurrentFrame].ImageAvailable, nullptr, &mImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR)
			Resize();
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			assert(false);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		VK_ASSERT(vkBeginCommandBuffer(mFrames[mImageIndex].CommandBuffer, &beginInfo), "Failed to begin command buffer");

		sBound = this;
	}

	/**
	 * @brief Submits the current framebuffer to the graphics and presentation queues
	 */
	void VkSurface::End()
	{
		vkEndCommandBuffer(mFrames[mImageIndex].CommandBuffer);

		VkSemaphore waitSemaphores[]{ mFrames[mCurrentFrame].ImageAvailable };
		VkSemaphore signalSemaphores[]{ mFrames[mCurrentFrame].RenderFinished };
		const VkPipelineStageFlags waitStages[]{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VK_ASSERT(vkResetFences(VkContext::Get().LogicalDevice(), 1, &mFrames[mCurrentFrame].InFlight), "Failed to reset fence");

		VkSubmitInfo submitInfo{};
		submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount	= _countof(waitSemaphores);
		submitInfo.pWaitSemaphores		= waitSemaphores;
		submitInfo.pWaitDstStageMask	= waitStages;
		submitInfo.signalSemaphoreCount = _countof(signalSemaphores);
		submitInfo.pSignalSemaphores	= signalSemaphores;
		submitInfo.commandBufferCount	= 1;
		submitInfo.pCommandBuffers		= &mFrames[mImageIndex].CommandBuffer;
		VK_ASSERT(vkQueueSubmit(VkContext::Get().Device().GraphicsQueue(), 1, &submitInfo, mFrames[mCurrentFrame].InFlight), "Failed to submit to graphics queue");

		VkSwapchainKHR swapchains[]{ mSwapchain };

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType				= VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount	= _countof(signalSemaphores);
		presentInfo.pWaitSemaphores		= signalSemaphores;
		presentInfo.swapchainCount		= _countof(swapchains);
		presentInfo.pSwapchains			= swapchains;
		presentInfo.pImageIndices		= &mImageIndex;
		presentInfo.pResults			= nullptr;

		const VkResult result = vkQueuePresentKHR(VkContext::Get().Device().PresentQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			Resize();
		else if (result != VK_SUCCESS)
			assert(false);

		mCurrentFrame = (mCurrentFrame + 1) % MaxFramesInFlight;
	}

	void VkSurface::SetInterface()
	{
		sCreateFunc = CreateFuncVulkan;
	}

	/**
	 * @brief Gets all the current surface`s capabilities
	 */
	void VkSurface::QuerySupport()
	{
		VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkContext::Get().PhysicalDevice(), mSurface, &capabilities.Capabilities), "Failed to query surface capabilities");

		u32 formatCount{ 0 };
		VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(VkContext::Get().PhysicalDevice(), mSurface, &formatCount, nullptr), "Failed to query surface formats");
		capabilities.Formats.resize(formatCount);
		VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(VkContext::Get().PhysicalDevice(), mSurface, &formatCount, capabilities.Formats.data()), "Failed to query surface formats");

		u32 presentModeCount{ 0 };
		VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(VkContext::Get().PhysicalDevice(), mSurface, &presentModeCount, nullptr), "Failed to query surface present modes");
		capabilities.PresentModes.resize(presentModeCount);
		VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(VkContext::Get().PhysicalDevice(), mSurface, &presentModeCount, capabilities.PresentModes.data()), "Failed to query surface present modes");

		assert(!capabilities.Formats.empty() && !capabilities.PresentModes.empty());
	}

	/**
	 * @brief Chooses the best settings from all the available capabilities
	 */
	void VkSurface::ChooseSettings()
	{
		QuerySupport();

		bool found{ false };
		for (const auto& availableFormat : capabilities.Formats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
			{
				mFormat = availableFormat;
				found = true;
			}
		}
		if (!found)
			mFormat = capabilities.Formats[0];

		found = false;
		for (const auto& availablePresentMode : capabilities.PresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				mPresentMode = availablePresentMode;
				found = true;
			}
		}
		if (!found)
			mPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	}

	/**
	 * @brief Chooses the best width and height for the current surface`s state
	 */
	void VkSurface::ChooseExtent()
	{
		QuerySupport();

		if (capabilities.Capabilities.currentExtent.width != UINT32_MAX)
			mExtent = capabilities.Capabilities.currentExtent;
		else
		{
			int width, height;
			glfwGetFramebufferSize(mWindow->Native(), &width, &height);

			VkExtent2D actualExtent
			{
				static_cast<u32>(width),
				static_cast<u32>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.Capabilities.minImageExtent.width, capabilities.Capabilities.maxImageExtent.width);
			actualExtent.width = std::clamp(actualExtent.height, capabilities.Capabilities.minImageExtent.height, capabilities.Capabilities.maxImageExtent.height);

			mExtent = actualExtent;
		}
	}

	/**
	 * @brief Creates a swapchain and gets its images
	 */
	void VkSurface::CreateSwapchain()
	{
		const u32 indices[]{ VkContext::Get().Device().GraphicsFamily(), VkContext::Get().Device().PresentFamily()};
		const bool sharedQueues{ indices[0] == indices[1] };

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface					= mSurface;
		createInfo.minImageCount			= BufferCount;
		createInfo.imageFormat				= mFormat.format;
		createInfo.imageColorSpace			= mFormat.colorSpace;
		createInfo.imageExtent				= mExtent;
		createInfo.imageArrayLayers			= 1;
		createInfo.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode			= sharedQueues ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount	= sharedQueues ? 0 : 2;
		createInfo.pQueueFamilyIndices		= sharedQueues ? nullptr : indices;
		createInfo.preTransform				= capabilities.Capabilities.currentTransform;
		createInfo.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode				= mPresentMode;
		createInfo.clipped					= true;
		createInfo.oldSwapchain				= nullptr;

		VK_ASSERT(vkCreateSwapchainKHR(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mSwapchain), "Failed to create swapchain");

		u32 imageCount{ 0 };
		VK_ASSERT(vkGetSwapchainImagesKHR(VkContext::Get().LogicalDevice(), mSwapchain, &imageCount, nullptr), "Failed to get swapchain images");
		std::vector<VkImage> mImages{ imageCount };
		VK_ASSERT(vkGetSwapchainImagesKHR(VkContext::Get().LogicalDevice(), mSwapchain, &imageCount, mImages.data()), "Failed to get swapchain images");

		for (u32 i{ 0 }; i < BufferCount; i++)
			mFrames[i].Image = mImages[i];
	}

	/**
	 * @brief Creates image views from the swapchain`s images
	 */
	void VkSurface::CreateImageViews()
	{
		for (u32 i{ 0 }; i < BufferCount; i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image							= mFrames[i].Image;
			createInfo.viewType							= VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format							= mFormat.format;
			createInfo.components.r						= VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g						= VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b						= VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a						= VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel	= 0;
			createInfo.subresourceRange.levelCount		= 1;
			createInfo.subresourceRange.baseArrayLayer	= 0;
			createInfo.subresourceRange.layerCount		= 1;

			VK_ASSERT(vkCreateImageView(VkContext::Get().LogicalDevice(), &createInfo, nullptr, &mFrames[i].ImageView), "Failed to create swapchain image view")
		}
	}

	/**
	 * @brief Creates a command pool and allocates a command buffer for each frame
	 */
	void VkSurface::CreateCommandBuffers()
	{
		std::vector<VkCommandBuffer> commandBuffers{ BufferCount };

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool			= mCommandPool;
		allocInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount	= static_cast<u32>(mFrames.size());
		VK_ASSERT(vkAllocateCommandBuffers(VkContext::Get().LogicalDevice(), &allocInfo, commandBuffers.data()), "Failed to allocate command buffers");

		for (u32 i{ 0 }; i < BufferCount; i++)
			mFrames[i].CommandBuffer = commandBuffers[i];
	}

	/**
	 * @brief Creates fences and semaphores used in framebuffer syncronization
	 */
	void VkSurface::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (u32 i{ 0 }; i < BufferCount; i++)
		{
			VK_ASSERT(vkCreateSemaphore(VkContext::Get().LogicalDevice(), &semaphoreInfo, nullptr, &mFrames[i].ImageAvailable), "Failed to create semaphore")
			VK_ASSERT(vkCreateSemaphore(VkContext::Get().LogicalDevice(), &semaphoreInfo, nullptr, &mFrames[i].RenderFinished), "Failed to create semaphore")
			VK_ASSERT(vkCreateFence(VkContext::Get().LogicalDevice(), &fenceInfo, nullptr, &mFrames[i].InFlight), "Failed to create fence")
		}
	}

	void VkSurface::Resize()
	{
		vkDeviceWaitIdle(VkContext::Get().LogicalDevice());

		std::vector<VkCommandBuffer> commandBuffers{ BufferCount };
		for (u32 i{ 0 }; i < BufferCount; i++)
			commandBuffers[i] = mFrames[i].CommandBuffer;

		vkFreeCommandBuffers(VkContext::Get().LogicalDevice(), mCommandPool, static_cast<uint32_t>(mFrames.size()), commandBuffers.data());

		for (u32 i{ 0 }; i < BufferCount; i++)
			vkDestroyImageView(VkContext::Get().LogicalDevice(), mFrames[i].ImageView, nullptr);

		vkDestroySwapchainKHR(VkContext::Get().LogicalDevice(), mSwapchain, nullptr);

		QuerySupport();
		ChooseExtent();

		CreateSwapchain();
		CreateImageViews();
		CreateCommandBuffers();
	}
}
