#pragma once
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.hpp>

#include "VkSurface.h"
#include "../../Common/Math.h"

namespace FrostEngine::Vulkan
{
	struct Vertex
	{
		glm::vec2 Position;
		glm::vec3 Color;
	};

	class VkBuffer
	{
	public:
		explicit VkBuffer(uint64_t size, vk::BufferUsageFlags usage, vma::MemoryUsage vmaUsage);
		~VkBuffer();

		constexpr vk::Buffer Buffer() const { return  mBuffer; }

		void InsertData(const void* data, uint64_t size = 0, uint64_t offset = 0) const;

	protected:
		vk::Buffer mBuffer{ nullptr };
		vma::Allocation mAllocation{ nullptr };
		uint64_t mSize{ 0 };
	};

	class VkVertexBuffer : public VkBuffer
	{
	public:
		explicit VkVertexBuffer(const Vertex* vertices, uint32_t vertexCount);

		void Bind(const vk::CommandBuffer& cmd) const;

		static vk::VertexInputBindingDescription BindingDescription();
		static std::array<vk::VertexInputAttributeDescription, 2> AttributeDescriptions();

	private:
		uint32_t mCount{ 0 };
	};

	class VkIndexBuffer : public VkBuffer
	{
	public:
		explicit VkIndexBuffer(const uint32_t* indices, uint32_t indicesCount);

		void Bind(const vk::CommandBuffer& cmd) const;

	private:
		uint32_t mCount{ 0 };
	};

	class VkUniformBuffer
	{
	public:
		explicit VkUniformBuffer(uint32_t size);

		[[nodiscard]] constexpr uint32_t Size() const { return mSize; }
		[[nodiscard]] constexpr vk::Buffer Buffer(uint32_t frame) const { return mBuffers[frame]; }

		void Update(const void* data, uint32_t frame) const;

	private:
		std::vector<vk::Buffer> mBuffers{ VkSurface::BufferCount };
		std::vector<vma::Allocation> mAllocations{ VkSurface::BufferCount };

		uint32_t mSize{};
	};
}