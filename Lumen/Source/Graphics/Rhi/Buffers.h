#pragma once
#include <glm/glm.hpp>

#include "Core/Types.h"

namespace Lumen::Graphics
{
	class Surface;

	struct Vertex
	{
		glm::vec2 Position;
		glm::vec3 Color;
	};

	class VertexBuffer
	{
	public:
		static VertexBuffer* Create(const Vertex* vertices, u32 vertexCount);
		virtual ~VertexBuffer() = default;

		virtual void Bind(Surface* target) const = 0;

	protected:
		static VertexBuffer* (*sCreateFunc)(const Vertex*, u32);
	};

	class IndexBuffer
	{
	public:
		static IndexBuffer* Create(const u32* indices, u32 indexCount);
		virtual ~IndexBuffer() = default;

		virtual void Bind(Surface* target) const = 0;
		virtual void Draw(Surface* target) const = 0;

	protected:
		static IndexBuffer* (*sCreateFunc)(const u32*, u32);
	};
}
