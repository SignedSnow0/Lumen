#pragma once
#include "VkDevice.h"
#include "Graphics/Rhi/GraphicsContext.h"
#pragma comment(lib, "vulkan-1.lib")

namespace Lumen::Graphics::Vulkan
{
	class VkContext final : public GraphicsContext
	{
	public:
		explicit VkContext();
		~VkContext() override;

		[[nodiscard]] constexpr const VkDevice& Device() const { return mDevice; }
		[[nodiscard]] constexpr ::VkDevice LogicalDevice() const { return mDevice.Device(); }
		[[nodiscard]] constexpr VkPhysicalDevice PhysicalDevice() const { return mDevice.PhysicalDevice(); }
		[[nodiscard]] constexpr VkInstance Instance() const { return mDevice.Instance(); }

		void Init() override;

		void WaitIdle() const override;

		static void SetInterface();
		static VkContext& Get() { return *sInstance; }

	private:
		VkDevice mDevice{};

		static VkContext* sInstance;
	};
}
