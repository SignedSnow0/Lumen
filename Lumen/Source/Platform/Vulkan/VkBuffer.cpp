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
			VkCommandPool pool{};//todo single pool
			VK_ASSERT(vkCreateCommandPool(VkContext::Get().LogicalDevice(), &poolInfo, nullptr, &pool), "Failed to create command pool");

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandBufferCount = 1;
			allocInfo.commandPool = pool;

			VkCommandBuffer commandBuffer;
			VK_ASSERT(vkAllocateCommandBuffers(VkContext::Get().LogicalDevice(), &allocInfo, &commandBuffer), "Failed to allocate command buffer");

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			VK_ASSERT(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin command buffer");
			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

			vkEndCommandBuffer(commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			VK_ASSERT(vkQueueSubmit(VkContext::Get().Device().GraphicsQueue(), 1, &submitInfo, nullptr), "Failed to submit copy op");
			vkQueueWaitIdle(VkContext::Get().Device().GraphicsQueue());

			vkFreeCommandBuffers(VkContext::Get().LogicalDevice(), pool, 1, &commandBuffer);
			vkDestroyCommandPool(VkContext::Get().LogicalDevice(), pool, nullptr);
		}
		
	}

	VkBuffer::VkBuffer(uint64_t size, VkBufferUsageFlags usage, VmaMemoryUsage vmaUsage)
		: mSize{ size }
	{
		VkBufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.size = size;
		createInfo.usage = usage;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = vmaUsage;

		VmaAllocationInfo resultInfo{};
		auto result = vmaCreateBuffer(VkContext::Get().Device().Allocator(), &createInfo, &allocInfo, &mBuffer, &mAllocation, &resultInfo);
	}

	VkBuffer::~VkBuffer()
	{
		vmaFreeMemory(VkContext::Get().Device().Allocator(), mAllocation);
		vkDestroyBuffer(VkContext::Get().LogicalDevice(), mBuffer, nullptr);
	}

	void VkBuffer::InsertData(const void* data, uint64_t size, uint64_t offset) const
	{
		void* gpuMem;
		vmaMapMemory(VkContext::Get().Device().Allocator(), mAllocation, &gpuMem);
		memcpy(static_cast<uint8_t*>(gpuMem) + offset, data, size ? size : mSize);
		vmaUnmapMemory(VkContext::Get().Device().Allocator(), mAllocation);
	}

	VkVertexBuffer::VkVertexBuffer(const Vertex* vertices, uint32_t vertexCount)
		: VkBuffer{ sizeof(Vertex) * vertexCount, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY }, mCount{ vertexCount }
	{ 
		const VkBuffer stagingBuffer{ sizeof(Vertex) * vertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY };
		stagingBuffer.InsertData(vertices);

		CopyBuffer(stagingBuffer.Buffer(), mBuffer, mSize);
	}

	void VkVertexBuffer::Bind(const VkCommandBuffer& cmd) const
	{
		VkDeviceSize offsets[]{ 0 };
		vkCmdBindVertexBuffers(cmd, 0, 1, &mBuffer, offsets);
	}

	VkVertexInputBindingDescription VkVertexBuffer::BindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	std::array<VkVertexInputAttributeDescription, 2> VkVertexBuffer::AttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, Color);

		return attributeDescriptions;
	}

	VkIndexBuffer::VkIndexBuffer(const uint32_t* indices, uint32_t indicesCount)
		: VkBuffer{ sizeof(uint32_t) * indicesCount, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY }, mCount{ indicesCount }
	{
		const VkBuffer stagingBuffer{ sizeof(Vertex) * indicesCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY };
		stagingBuffer.InsertData(indices);

		CopyBuffer(stagingBuffer.Buffer(), mBuffer, mSize);
	}

	void VkIndexBuffer::Bind(const VkCommandBuffer& cmd) const
	{
		vkCmdBindIndexBuffer(cmd, mBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	VkUniformBuffer::VkUniformBuffer(uint32_t size)
		: mSize{ size }
	{
		VkBufferCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		createInfo.size = size;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		for(uint32_t i{ 0 }; i < mBuffers.size(); i++)
			vmaCreateBuffer(VkContext::Get().Device().Allocator(), &createInfo, &allocInfo, &mBuffers[i], &mAllocations[i], nullptr);
	}

	void VkUniformBuffer::Update(const void* data, uint32_t frame) const
	{
		void* map;
		vmaMapMemory(VkContext::Get().Device().Allocator(), mAllocations[frame], &map);
		memcpy(map, data, mSize);
		vmaUnmapMemory(VkContext::Get().Device().Allocator(), mAllocations[frame]);
	}
}
