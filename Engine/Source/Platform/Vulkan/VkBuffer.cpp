#include "VkBuffer.h"

#include <array>

#include "VkContext.h"

namespace Lumen::Graphics::Vulkan
{
	namespace 
	{
		void CopyBuffer(const ::VkBuffer src, const ::VkBuffer dst, const VkDeviceSize size)
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

		VertexBuffer* CreateFuncVulkan(const Vertex* vertices, const u32 vertexCount)
		{
			return new VkVertexBuffer{ vertices, vertexCount };
		}

		IndexBuffer* CreateFuncVulkan(const u32 * indices, const u32 indexCount)
		{
			return new VkIndexBuffer{ indices, indexCount };
		}
	}

	/**
	 * \brief Creates a generic buffer
	 * \param size Byte size of the buffer
	 * \param usage Usage for the buffer
	 * \param vmaUsage Vma usage for the buffer
	 */
	VkBuffer::VkBuffer(const u64 size, const VkBufferUsageFlags usage, const VmaMemoryUsage vmaUsage)
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

	/**
	 * \brief Returns the native buffer
	 */
	::VkBuffer VkBuffer::Buffer() const { return mBuffer; }

	/**
	 * \brief Inserts data into the buffer
	 * \param data Pointer to the data in the cpu
	 * \param size Byte size of the data (if 0 then the whole buffer is filled)
	 * \param offset Offset in the gpu buffer
	 */
	void VkBuffer::InsertData(const void* data, const u64 size, const u64 offset) const
	{
		void* gpuMem;
		vmaMapMemory(VkContext::Get().Device().Allocator(), mAllocation, &gpuMem);
		memcpy(static_cast<uint8_t*>(gpuMem) + offset, data, size ? size : mSize);
		vmaUnmapMemory(VkContext::Get().Device().Allocator(), mAllocation);
	}

	/**
	 * \brief Creates a new vertex buffer
	 * \param vertices Pointer to the vertices in the cpu
	 * \param vertexCount Number of vertices
	 */
	VkVertexBuffer::VkVertexBuffer(const Vertex* vertices, const u32 vertexCount)
		: VkBuffer{ sizeof(Vertex) * vertexCount, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY }, mCount{ vertexCount }
	{ 
		const VkBuffer stagingBuffer{ sizeof(Vertex) * vertexCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY };
		stagingBuffer.InsertData(vertices);

		CopyBuffer(stagingBuffer.Buffer(), mBuffer, mSize);
	}

	/**
	 * \brief Sets this vertex buffer as the current vertex buffer
	 * \param target The target to set the vertex buffer to
	 */
	void VkVertexBuffer::Bind(Surface* target) const
	{
		VkCommandBuffer cmd = dynamic_cast<VkSurface*>(target)->CommandBuffer();
		constexpr VkDeviceSize offsets[]{ 0 };
		vkCmdBindVertexBuffers(cmd, 0, 1, &mBuffer, offsets);
	}

	/**
	 * \brief Returns the default vertex binding description
	 */
	VkVertexInputBindingDescription VkVertexBuffer::BindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding		= 0;
		bindingDescription.stride		= sizeof(Vertex);
		bindingDescription.inputRate	= VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	/**
	 * \brief Returns the default vertex attribute descriptions
	 */
	std::array<VkVertexInputAttributeDescription, 3> VkVertexBuffer::AttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
		attributeDescriptions[0].binding	= 0;
		attributeDescriptions[0].location	= 0;
		attributeDescriptions[0].format		= VK_FORMAT_R32G32B32_SFLOAT;
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

	/**
	 * \brief Creates a new index buffer
	 * \param indices Pointer to the indices in the cpu
	 * \param indicesCount Number of indices
	 */
	VkIndexBuffer::VkIndexBuffer(const u32* indices, const u32 indicesCount)
		: VkBuffer{ sizeof(u32) * indicesCount, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY }, mCount{ indicesCount }
	{
		const VkBuffer stagingBuffer{ sizeof(Vertex) * indicesCount, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY };
		stagingBuffer.InsertData(indices);

		CopyBuffer(stagingBuffer.Buffer(), mBuffer, mSize);
	}

	/**
	 * \brief Sets this index buffer as the current index buffer
	 * \param target The target to set the index buffer to
	 */
	void VkIndexBuffer::Bind(Surface* target) const
	{
		VkCommandBuffer cmd = dynamic_cast<VkSurface*>(target)->CommandBuffer();
		vkCmdBindIndexBuffer(cmd, mBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	/**
	 * \brief Draws to the target using the last vertex and index buffer bound
	 * \param target The target to draw to
	 */
	void VkIndexBuffer::Draw(Surface* target) const
	{
		VkCommandBuffer cmd = dynamic_cast<VkSurface*>(target)->CommandBuffer();
		vkCmdDrawIndexed(cmd, mCount, 1, 0, 0, 0);
	}

	void VkIndexBuffer::SetInterface()
	{
		sCreateFunc = CreateFuncVulkan;
	}

	/**
	 * \brief Creates a new uniform buffer
	 * \param size Byte size of the buffer
	 */
	VkUniformBuffer::VkUniformBuffer(const u32 size)
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

	/**
	 * \brief Returns the byte size of the buffer
	 */
	u32 VkUniformBuffer::Size() const { return mSize; }

	/**
	 * \brief Returns the native buffer
	 * \param frame The frame to get the buffer from
	 */
	::VkBuffer VkUniformBuffer::Buffer(const u32 frame) const { return mBuffers[frame]; }

	/**
	 * \brief Sets the data of the buffer
	 * \param data Pointer to the uniform data in the cpu
	 * \param frame The frame to set the data to
	 */
	void VkUniformBuffer::Update(const void* data, const u32 frame) const
	{
		void* gpuMem;
		vmaMapMemory(VkContext::Get().Device().Allocator(), mAllocations[frame], &gpuMem);
		memcpy(gpuMem, data, mSize);
		vmaUnmapMemory(VkContext::Get().Device().Allocator(), mAllocations[frame]);
	}
}
