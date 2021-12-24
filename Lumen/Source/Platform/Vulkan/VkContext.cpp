﻿#include "VkContext.h"

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
		sInstance = this;
	}

	VkContext::~VkContext()
	{
	}

	void VkContext::Init()
	{
		mDevice.Init();
	}

	void VkContext::WaitIdle() const
	{
		vkDeviceWaitIdle(mDevice.Device());
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
