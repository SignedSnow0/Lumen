#pragma once
#include <optional>

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "Core/Types.h"

namespace Lumen::Graphics::Vulkan
{
#ifdef _DEBUG
#include <cassert>
#define VK_ASSERT(func, msg)\
if ((func) != VK_SUCCESS)\
{\
	assert(false && (msg));\
}
#else
#define VK_ASSERT(func, msg) func
#endif

	struct QueueFamilyIndices
	{
		std::optional<u32> Graphics;
		std::optional<u32> Present;

		[[nodiscard]] bool IsComplete() const { return Graphics.has_value() && Present.has_value(); }
	};

	class VkDevice
	{
	public:
		VkDevice() = default;
		~VkDevice();

		[[nodiscard]] VkPhysicalDeviceProperties Properties() const;
		[[nodiscard]] u32 GraphicsFamily() const;
		[[nodiscard]] u32 PresentFamily() const;
		[[nodiscard]] ::VkDevice Device() const;
		[[nodiscard]] VkPhysicalDevice PhysicalDevice() const;
		[[nodiscard]] VkInstance Instance() const;
		[[nodiscard]] VkQueue GraphicsQueue() const;
		[[nodiscard]] VkQueue PresentQueue() const;
		[[nodiscard]] VmaAllocator Allocator() const;

		void Init();
		void Release();

	private:
		void CreateInstance();
		void CreateDebugMessenger();
		b8 FindQueueFamilies(VkPhysicalDevice device, QueueFamilyIndices& indices);
		b8 IsDeviceSuitable(VkPhysicalDevice device, QueueFamilyIndices& indices);
		void CreatePhysicalDevice();
		void CreateLogicalDevice();
		void CreateAllocator();

		QueueFamilyIndices			mIndices;
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
