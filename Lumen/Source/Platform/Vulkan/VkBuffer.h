#pragma once
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <glm/glm.hpp>

#include "VkSurface.h"

namespace Lumen::Graphics::Vulkan
{
	struct Vertex
	{
		glm::vec2 Position;
		glm::vec3 Color;
	};

	class VkBuffer
	{
	public:
		explicit VkBuffer(uint64_t size, VkBufferUsageFlags usage, VmaMemoryUsage vmaUsage);
		~VkBuffer();

		[[nodiscard]] constexpr ::VkBuffer Buffer() const { return  mBuffer; }

		void InsertData(const void* data, uint64_t size = 0, uint64_t offset = 0) const;

	protected:
		::VkBuffer mBuffer{ nullptr };
		VmaAllocation mAllocation{ nullptr };
		uint64_t mSize{ 0 };
	};

	class VkVertexBuffer : public VkBuffer
	{
	public:
		explicit VkVertexBuffer(const Vertex* vertices, uint32_t vertexCount);

		void Bind(const VkCommandBuffer& cmd) const;

		static VkVertexInputBindingDescription BindingDescription();
		static std::array<VkVertexInputAttributeDescription, 2> AttributeDescriptions();

	private:
		uint32_t mCount{ 0 };
	};

	class VkIndexBuffer : public VkBuffer
	{
	public:
		explicit VkIndexBuffer(const uint32_t* indices, uint32_t indicesCount);

		void Bind(const VkCommandBuffer& cmd) const;

	private:
		uint32_t mCount{ 0 };
	};

	class VkUniformBuffer
	{
	public:
		explicit VkUniformBuffer(uint32_t size);

		[[nodiscard]] constexpr uint32_t Size() const { return mSize; }
		[[nodiscard]] constexpr ::VkBuffer Buffer(uint32_t frame) const { return mBuffers[frame]; }

		void Update(const void* data, uint32_t frame) const;

	private:
		std::vector<::VkBuffer> mBuffers{ VkSurface::BufferCount };
		std::vector<VmaAllocation> mAllocations{ VkSurface::BufferCount };

		uint32_t mSize{};
	};
}