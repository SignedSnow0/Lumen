#include "VkBuffer.h"

#include <array>

#include "VkContext.h"

namespace Lumen::Graphics::Vulkan
{
	namespace 
	{
		void CopyBuffer(::VkBuffer src, ::VkBuffer dst, VkDeviceSize size)
		{
			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			VkCommandPool pool{};//todo single pool
			VK_ASSERT(vkCreateCommandPool(VkContext::Get().LogicalDevice(), &poolInfo, nullptr, &pool), "Failed to create command pool");

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount	= 1;
			allocInfo.commandPool			= pool;

			VkCommandBuffer commandBuffer;
			VK_ASSERT(vkAllocateCommandBuffers(VkContext::Get().LogicalDevice(), &allocInfo, &commandBuffer), "Failed to allocate command buffer");

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin command buffer");
			VkBufferCopy copyRegion{};
			copyRegion.srcOffset	= 0;
			copyRegion.dstOffset	= 0;
			copyRegion.size			= size;
			vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType				= VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount	= 1;
			submitInfo.pCommandBuffers		= &commandBuffer;

			VK_ASSERT(vkQueueSubmit(VkContext::Get().Device().GraphicsQueue(), 1, &submitInfo, nullptr), "Failed to submit copy op");
			vkQueueWaitIdle(VkContext::Get().Device().GraphicsQueue());

			vkFreeCommandBuffers(VkContext::Get().LogicalDevice(), pool, 1, &commandBuffer);
			vkDestroyCommandPool(VkContext::Get().LogicalDevice(), pool, nullptr);
		}

		VertexBuffer* CreateFuncVulkan(const Vertex* vertices, u32 vertexCount)
		{
			return new VkVertexBuffer{ vertices, vertexCount };
		}

		IndexBuffer* CreateFuncVulkan(const u32 * indices, u32 indexCount)
		{
			return new VkIndexBuffer{ indices, indexCount };
		}
	}

	VkBuffer::VkBuffer(u64 size, VkBufferUsageFlags usage, VmaMemoryUsage vmaUsage)
		: mSize{ size }
	{
		VkBufferCreateInfo createInfo{};
		createInfo.sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size			= size;
		createInfo.usage		= usage;
		createInfo.sharingMode	= VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = vmaUsage;

		VmaAllocationInfo resultInfo{};
		auto result = vmaCreateBuffer(VkContext::Get().Device().Allocator(), &createInfo, &allocInfo, &mBuffer, &mAllocation, &resultInfo);
	}

	VkBuffer::~VkBuffer()
	{
		vkDestroyBuffer(VkContext::Get().LogicalDevice(), mBuffer, nullptr);
	}

	void VkBuffer::InsertData(const void* data, u64 size, u64 offset) const
	{
		void* gpuMem;
		vmaMapMemory(VkContext::Get().Device().Allocator(), mAllocation, &gpuMem);
		memcpy(static_cast<uint8_t*>(gpuMem) + offset, data, size ? size : mSize);
		vmaUnmapMemory(VkContext::Get().Device().Allocator(), mAllocation);
	}

	VkVertexBuffer::VkVertexBuffer(const Vertex* vertices, u32 vertexCount)
		: VkBuffer{ sizeof(Vertex) * vertexCount, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY }, mCount{ vertexCount }
	{ 
		const VkBuffer stagingBuffer{ sizeof(Vertex) * vertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY };
		stagingBuffer.InsertData(vertices);

		CopyBuffer(stagingBuffer.Buffer(), mBuffer, mSize);
	}

	void VkVertexBuffer::Bind(Surface* target) const
	{
		VkCommandBuffer cmd = dynamic_cast<VkSurface*>(target)->CommandBuffer();
		constexpr VkDeviceSize offsets[]{ 0 };
		vkCmdBindVertexBuffers(cmd, 0, 1, &mBuffer, offsets);
	}

	VkVertexInputBindingDescription VkVertexBuffer::BindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding		= 0;
		bindingDescription.stride		= sizeof(Vertex);
		bindingDescription.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	std::array<VkVertexInputAttributeDescription, 3> VkVertexBuffer::AttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
		attributeDescriptions[0].binding	= 0;
		attributeDescriptions[0].location	= 0;
		attributeDescriptions[0].format		= VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset		= offsetof(Vertex, Position);

		attributeDescriptions[1].binding	= 0;
		attributeDescriptions[1].location	= 1;
		attributeDescriptions[1].format		= VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset		= offsetof(Vertex, Color);

		attributeDescriptions[2].binding	= 0;
		attributeDescriptions[2].location	= 2;
		attributeDescriptions[2].format		= VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset		= offsetof(Vertex, TextureCoordinates);

		return attributeDescriptions;
	}

	void VkVertexBuffer::SetInterface()
	{
		sCreateFunc = CreateFuncVulkan;
	}

	VkIndexBuffer::VkIndexBuffer(const u32* indices, u32 indicesCount)
		: VkBuffer{ sizeof(u32) * indicesCount, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY }, mCount{ indicesCount }
	{
		const VkBuffer stagingBuffer{ sizeof(Vertex) * indicesCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY };
		stagingBuffer.InsertData(indices);

		CopyBuffer(stagingBuffer.Buffer(), mBuffer, mSize);
	}

	void VkIndexBuffer::Bind(Surface* target) const
	{
		VkCommandBuffer cmd = dynamic_cast<VkSurface*>(target)->CommandBuffer();
		vkCmdBindIndexBuffer(cmd, mBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void VkIndexBuffer::Draw(Surface* target) const
	{
		VkCommandBuffer cmd = dynamic_cast<VkSurface*>(target)->CommandBuffer();
		vkCmdDrawIndexed(cmd, mCount, 1, 0, 0, 0);
	}

	void VkIndexBuffer::SetInterface()
	{
		sCreateFunc = CreateFuncVulkan;
	}

	VkUniformBuffer::VkUniformBuffer(u32 size)
		: mSize{ size }
	{
		VkBufferCreateInfo createInfo{};
		createInfo.sType	= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.usage	= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		createInfo.size		= size;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		for (u32 i{ 0 }; i < mBuffers.size(); i++)
			vmaCreateBuffer(VkContext::Get().Device().Allocator(), &createInfo, &allocInfo, &mBuffers[i], &mAllocations[i], nullptr);
	}

	VkUniformBuffer::~VkUniformBuffer()
	{
		for(const auto& buffer : mBuffers)
			vkDestroyBuffer(VkContext::Get().LogicalDevice(), buffer, nullptr);
	}

	void VkUniformBuffer::Update(const void* data, u32 frame) const
	{
		void* gpuMem;
		vmaMapMemory(VkContext::Get().Device().Allocator(), mAllocations[frame], &gpuMem);
		memcpy(gpuMem, data, mSize);
		vmaUnmapMemory(VkContext::Get().Device().Allocator(), mAllocations[frame]);
	}
}
