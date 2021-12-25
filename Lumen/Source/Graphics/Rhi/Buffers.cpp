#include "Buffers.h"

namespace Lumen::Graphics
{
	VertexBuffer* (*VertexBuffer::sCreateFunc)(const Vertex*, u32) = nullptr;

	VertexBuffer* VertexBuffer::Create(const Vertex* vertices, u32 vertexCount)
	{
		assert(sCreateFunc && "No graphics api set!");
		return sCreateFunc(vertices, vertexCount);
	}



	IndexBuffer* (*IndexBuffer::sCreateFunc)(const u32*, u32) = nullptr;

	IndexBuffer* IndexBuffer::Create(const u32* indices, u32 indexCount)
	{
		assert(sCreateFunc && "No graphics api set!");
		return sCreateFunc(indices, indexCount);
	}
}
