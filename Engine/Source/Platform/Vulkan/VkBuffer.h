#pragma once
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "VkSurface.h"
#include "Graphics/Rhi/Buffers.h"

namespace Lumen::Graphics::Vulkan
{
	class VkBuffer
	{
	public:
		explicit VkBuffer(u64 size, VkBufferUsageFlags usage, VmaMemoryUsage vmaUsage);
		~VkBuffer();

		[[nodiscard]] ::VkBuffer Buffer() const;

		void InsertData(const void* data, u64 size = 0, u64 offset = 0) const;

	protected:
		::VkBuffer mBuffer{ nullptr };
		VmaAllocation mAllocation{ nullptr };
		u64 mSize{ 0 };
	};

	class VkVertexBuffer : public VertexBuffer, public VkBuffer
	{
	public:
		explicit VkVertexBuffer(const Vertex* vertices, u32 vertexCount);

		void Bind(Surface* target) const override;

		static VkVertexInputBindingDescription BindingDescription();
		static std::array<VkVertexInputAttributeDescription, 3> AttributeDescriptions();

		static void SetInterface();

	private:
		u32 mCount{ 0 };
	};

	class VkIndexBuffer : public IndexBuffer, public VkBuffer
	{
	public:
		explicit VkIndexBuffer(const u32* indices, u32 indicesCount);

		void Bind(Surface* target) const override;
		void Draw(Surface* target) const override;

		static void SetInterface();

	private:
		u32 mCount{ 0 };
	};

	class VkUniformBuffer
	{
	public:
		VkUniformBuffer() = default;
		explicit VkUniformBuffer(u32 size);
		~VkUniformBuffer();

		u32 Size() const;
		::VkBuffer Buffer(u32 frame) const;

		void Update(const void* data, u32 frame) const;

	private:
		std::vector<::VkBuffer> mBuffers{ VkSurface::BufferCount };
		std::vector<VmaAllocation> mAllocations{ VkSurface::BufferCount };

		u32 mSize{ 0 };
	};
}
