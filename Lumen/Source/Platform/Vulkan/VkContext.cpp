#include "VkContext.h"

#include <iostream>

namespace Lumen::Graphics::Vulkan
{
	namespace 
	{
		GraphicsContext* CreateFuncVulkan()
		{
			return new VkContext{};
		}
	}

	VkContext* VkContext::sInstance{ nullptr };

	VkContext::VkContext()
	{
		std::cout << "Vulkan context created" << std::endl;
		sInstance = this;
	}

	VkContext::~VkContext()
	{
		std::cout << "Vulkan context destroyed" << std::endl;
	}

	void VkContext::Init()
	{
		std::cout << "Vulkan context init" << std::endl;
		mDevice.Init();
	}

	void VkContext::WaitIdle() const
	{

	}

	/**
	 * @brief Sets the interface`s Create() function to create a vulkan instance.\n
	 * Internal use only.
	 */
	void VkContext::SetInterface()
	{
		sCreateFunc = CreateFuncVulkan;
	}
}
