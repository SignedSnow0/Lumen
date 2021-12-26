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
		sInstance = this;
	}

	VkContext::~VkContext()
	{
		WaitIdle();
		vkDestroyCommandPool(mDevice.Device(), mSingleTimeCommandPool, nullptr);
	}

	void VkContext::Init()
	{
		mDevice.Init();

		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		VK_ASSERT(vkCreateCommandPool(mDevice.Device(), &createInfo, nullptr, &mSingleTimeCommandPool), "Failed to create single time command pool");
	}

	void VkContext::WaitIdle() const
	{
		vkDeviceWaitIdle(mDevice.Device());
	}

	VkCommandBuffer VkContext::StartCommand()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = mSingleTimeCommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(mDevice.Device(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to create single time commands");

		return commandBuffer;
	}

	void VkContext::EndCommand(VkCommandBuffer cmd)
	{
		vkEndCommandBuffer(cmd);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmd;

		vkQueueSubmit(mDevice.GraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(mDevice.GraphicsQueue());

		vkFreeCommandBuffers(mDevice.Device(), mSingleTimeCommandPool, 1, &cmd);
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
