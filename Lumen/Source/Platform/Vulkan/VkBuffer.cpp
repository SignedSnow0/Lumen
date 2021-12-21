#include "VkBuffer.h"

#include "VkGraphics.h"

namespace FrostEngine::Vulkan
{
	namespace 
	{
		void CopyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size)
		{
			vk::CommandPoolCreateInfo poolInfo{};
			vk::CommandPool pool{};//todo single pool
			VK_ASSERT(VkGraphics::Device().Device().createCommandPool(&poolInfo, nullptr, &pool), "Failed to create command pool");

			vk::CommandBufferAllocateInfo allocInfo{};
			allocInfo.level = vk::CommandBufferLevel::ePrimary;
			allocInfo.commandBufferCount = 1;
			allocInfo.commandPool = pool;

			vk::CommandBuffer commandBuffer;
			VK_ASSERT(VkGraphics::Device().Device().allocateCommandBuffers(&allocInfo, &commandBuffer), "Failed to allocate command buffer");

			vk::CommandBufferBeginInfo beginInfo{};
			beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

			VK_ASSERT(commandBuffer.begin(&beginInfo), "Failed to begin command buffer");
			vk::BufferCopy copyRegion{};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = size;
			commandBuffer.copyBuffer(src, dst, 1, &copyRegion);

			commandBuffer.end();

			vk::SubmitInfo submitInfo{};
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			VK_ASSERT(VkGraphics::Device().GraphicsQueue().submit(1, &submitInfo, nullptr), "Failed to submit copy op");
			VkGraphics::Device().GraphicsQueue().waitIdle();

			VkGraphics::Device().Device().freeCommandBuffers(pool, 1, &commandBuffer);
			VkGraphics::Device().Device().destroyCommandPool(pool, nullptr);
		}
		
	}

	VkBuffer::VkBuffer(uint64_t size, vk::BufferUsageFlags usage, vma::MemoryUsage vmaUsage)
		: mSize{ size }
	{
		vk::BufferCreateInfo createInfo{};
		createInfo.size = size;
		createInfo.usage = usage;
		createInfo.sharingMode = vk::SharingMode::eExclusive;

		vma::AllocationCreateInfo allocInfo{};
		allocInfo.usage = vmaUsage;

		vma::AllocationInfo resultInfo{};
		auto result = vmaCreateBuffer(VkGraphics::Device().Allocator(), reinterpret_cast<VkBufferCreateInfo*>(&createInfo), reinterpret_cast<VmaAllocationCreateInfo*>(&allocInfo), reinterpret_cast<::VkBuffer*>(&mBuffer), reinterpret_cast<VmaAllocation*>(&mAllocation), reinterpret_cast<VmaAllocationInfo*>(&resultInfo));
	}

	VkBuffer::~VkBuffer()
	{
		vmaFreeMemory(VkGraphics::Device().Allocator(), mAllocation);
		VkGraphics::Device().Device().destroyBuffer(mBuffer);
	}

	void VkBuffer::InsertData(const void* data, uint64_t size, uint64_t offset) const
	{
		void* gpuMem;
		vmaMapMemory(VkGraphics::Device().Allocator(), mAllocation, &gpuMem);
		memcpy(static_cast<uint8_t*>(gpuMem) + offset, data, size ? size : mSize);
		vmaUnmapMemory(VkGraphics::Device().Allocator(), mAllocation);
	}

	VkVertexBuffer::VkVertexBuffer(const Vertex* vertices, uint32_t vertexCount)
		: VkBuffer{ sizeof(Vertex) * vertexCount, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vma::MemoryUsage::eGpuOnly }, mCount{ vertexCount }
	{ 
		const VkBuffer stagingBuffer{ sizeof(Vertex) * vertexCount, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuOnly };
		stagingBuffer.InsertData(vertices);

		CopyBuffer(stagingBuffer.Buffer(), mBuffer, mSize);
	}

	void VkVertexBuffer::Bind(const vk::CommandBuffer& cmd) const
	{
		vk::DeviceSize offsets[]{ 0 };
		cmd.bindVertexBuffers(0, 1, &mBuffer, offsets);
	}

	vk::VertexInputBindingDescription VkVertexBuffer::BindingDescription()
	{
		vk::VertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = vk::VertexInputRate::eVertex;

		return bindingDescription;
	}

	std::array<vk::VertexInputAttributeDescription, 2> VkVertexBuffer::AttributeDescriptions()
	{
		std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions{};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32Sfloat;
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[1].offset = offsetof(Vertex, Color);

		return attributeDescriptions;
	}

	VkIndexBuffer::VkIndexBuffer(const uint32_t* indices, uint32_t indicesCount)
		: VkBuffer{ sizeof(uint32_t) * indicesCount, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vma::MemoryUsage::eGpuOnly }, mCount{ indicesCount }
	{
		const VkBuffer stagingBuffer{ sizeof(Vertex) * indicesCount, vk::BufferUsageFlagBits::eTransferSrc, vma::MemoryUsage::eCpuOnly };
		stagingBuffer.InsertData(indices);

		CopyBuffer(stagingBuffer.Buffer(), mBuffer, mSize);
	}

	void VkIndexBuffer::Bind(const vk::CommandBuffer& cmd) const
	{
		cmd.bindIndexBuffer(mBuffer, 0, vk::IndexType::eUint32);
	}

	VkUniformBuffer::VkUniformBuffer(uint32_t size)
		: mSize{ size }
	{
		vk::BufferCreateInfo createInfo{};
		createInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
		createInfo.size = size;

		vma::AllocationCreateInfo allocInfo{};
		allocInfo.usage = vma::MemoryUsage::eCpuToGpu;

		for(uint32_t i{ 0 }; i < mBuffers.size(); i++)
			vmaCreateBuffer(VkGraphics::Device().Allocator(), reinterpret_cast<VkBufferCreateInfo*>(&createInfo), reinterpret_cast<VmaAllocationCreateInfo*>(&allocInfo), reinterpret_cast<::VkBuffer*>(&mBuffers[i]), reinterpret_cast<VmaAllocation*>(&mAllocations[i]), nullptr);
	}

	void VkUniformBuffer::Update(const void* data, uint32_t frame) const
	{
		void* map;
		vmaMapMemory(VkGraphics::Device().Allocator(), mAllocations[frame], &map);
		memcpy(map, data, mSize);
		vmaUnmapMemory(VkGraphics::Device().Allocator(), mAllocations[frame]);
	}
}
