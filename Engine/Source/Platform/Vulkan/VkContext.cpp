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

	/**
	 * \brief Singleton instance of the graphics context
	 */
	VkContext* VkContext::sInstance{ nullptr };

	/**
	 * \brief Creates a new instance of the Vulkan context singleton
	 */
	VkContext::VkContext()
	{
		sInstance = this;
	}

	VkContext::~VkContext()
	{
		VkContext::WaitIdle();
		vkDestroyCommandPool(mDevice.Device(), mSingleTimeCommandPool, nullptr);
	}

	/**
	 * \brief Returns the device
	 */
	const VkDevice& VkContext::Device() const { return mDevice; }

	/**
	 * \brief Returns the native device
	 */
	::VkDevice VkContext::LogicalDevice() const { return mDevice.Device(); }

	/**
	 * \brief Returns the native physical device
	 */
	VkPhysicalDevice VkContext::PhysicalDevice() const { return mDevice.PhysicalDevice(); }

	/**
	 * \brief Returns the native instance
	 */
	VkInstance VkContext::Instance() const { return mDevice.Instance(); }

	/**
	 * \brief Returns a reference to the singleton instance
	 */
	VkContext& VkContext::Get() { return *sInstance; }

	/**
	 * \brief Initializes the graphics contexts resources
	 */
	void VkContext::Init()
	{
		mDevice.Init();

		VkCommandPoolCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		VK_ASSERT(vkCreateCommandPool(mDevice.Device(), &createInfo, nullptr, &mSingleTimeCommandPool), "Failed to create single time command pool");
	}

	/**
	 * \brief Waits for gpu operations to finish
	 */
	void VkContext::WaitIdle() const
	{
		vkDeviceWaitIdle(mDevice.Device());
	}

	/**
	 * \brief Starts a temporary command buffer
	 * \return The temporary command buffer
	 */
	VkCommandBuffer VkContext::StartCommand() const
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

	/**
	 * \brief Submits the temporary command buffer and executes it
	 * \param cmd The command buffer to submit created by StartCommand()
	 */
	void VkContext::EndCommand(VkCommandBuffer cmd) const
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
