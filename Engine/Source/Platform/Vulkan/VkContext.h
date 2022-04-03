#pragma once
#include <functional>

#include "VkDevice.h"
#include "Graphics/Rhi/GraphicsContext.h"
#pragma comment(lib, "vulkan-1.lib")

namespace Lumen::Graphics::Vulkan
{
	class VkContext : public GraphicsContext
	{
	public:
		explicit VkContext();
		~VkContext() override;

		[[nodiscard]] const VkDevice& Device() const;
		[[nodiscard]] ::VkDevice LogicalDevice() const;
		[[nodiscard]] VkPhysicalDevice PhysicalDevice() const;
		[[nodiscard]] VkInstance Instance() const;

		static VkContext& Get();

		void Init() override;

		void WaitIdle() const override;
		[[nodiscard]] VkCommandBuffer StartCommand() const;
		void EndCommand(VkCommandBuffer cmd) const;

		static void SetInterface();

	private:
		VkDevice mDevice{};
		VkCommandPool mSingleTimeCommandPool{};

		static VkContext* sInstance;
	};
}
