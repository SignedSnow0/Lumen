#pragma once
#include <optional>

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "Core/Types.h"

namespace Lumen::Graphics::Vulkan
{
#ifdef _DEBUG
#include <cassert>
#define VK_ASSERT(func, msg)		\
if ((func) != VK_SUCCESS)			\
{									\
	assert(false && (msg));			\
}
#else
#define VK_ASSERT(func, msg) func
#endif

	struct QueueFamiliyIndices
	{
		std::optional<u32> Graphics;
		std::optional<u32> Present;

		[[nodiscard]] bool IsComplete() const { return Graphics.has_value() && Present.has_value(); }
	};

	class VkDevice
	{
	public:
		VkDevice() = default;
		~VkDevice() { Release(); }

		[[nodiscard]] constexpr VkPhysicalDeviceProperties Properties() const	{ return mProperties; }
		[[nodiscard]] constexpr u32 GraphicsFamily() const						{ return mIndices.Graphics.value(); }
		[[nodiscard]] constexpr u32 PresentFamily() const						{ return mIndices.Present.value(); }
		[[nodiscard]] constexpr ::VkDevice Device() const						{ return mDevice; }
		[[nodiscard]] constexpr VkPhysicalDevice PhysicalDevice() const			{ return mPhysicalDevice; }
		[[nodiscard]] constexpr VkInstance Instance() const						{ return mInstance; }
		[[nodiscard]] constexpr VkQueue GraphicsQueue() const					{ return mGraphicsQueue; }
		[[nodiscard]] constexpr VkQueue PresentQueue() const					{ return mPresentQueue; }
		[[nodiscard]] VmaAllocator Allocator() const							{ return mAllocator; }

		void Init();
		void Release();

	private:
		void CreateInstance();
		void CreateDebugMessenger();
		bool FindQueueFamilies(VkPhysicalDevice device, QueueFamiliyIndices& indices);
		bool IsDeviceSuitable(VkPhysicalDevice device, QueueFamiliyIndices& indices);
		void CreatePhysicalDevice();
		void CreateLogicalDevice();
		void CreateAllocator();

		QueueFamiliyIndices			mIndices;
		VkPhysicalDevice			mPhysicalDevice{};
		::VkDevice					mDevice{};
		VkInstance					mInstance{};
		VkQueue						mGraphicsQueue{};
		VkQueue						mPresentQueue{};
		VkDebugUtilsMessengerEXT	mDebugMessenger{};
		VkPhysicalDeviceProperties  mProperties{};
		VmaAllocator				mAllocator{};
	};
}
