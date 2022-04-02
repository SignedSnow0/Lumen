#pragma once
#include "GraphicsTypes.h"
#include "Surface.h"
#include "Core/Types.h"

namespace Lumen::Graphics
{
	class VertexBuffer
	{
	public:
		/**
		 * \brief Use Create() instead
		 */
		VertexBuffer() = delete;
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer(VertexBuffer&&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;
		VertexBuffer& operator=(VertexBuffer&&) = delete;
		
		/**
		 * \brief Creates a new vertex buffer
		 * \param vertices Pointer to the beginning of the vertices buffer
		 * \param vertexCount Number of vertices in the buffer
		 * \return New vertex buffer
		 */
		static VertexBuffer* Create(const Vertex* vertices, u32 vertexCount);
		virtual ~VertexBuffer() = default;

		/**
		 * \brief Sets this buffer as the one to use for drawing operations
		 * \param target Target to bind to
		 */
		virtual void Bind(Surface* target) const = 0;

	protected:
		static VertexBuffer* (*sCreateFunc)(const Vertex*, u32);
	};

	class IndexBuffer
	{
	public:
		IndexBuffer() = delete;
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer(IndexBuffer&&) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;
		IndexBuffer& operator=(IndexBuffer&&) = delete;
		
		/**
		 * \brief Creates a new index buffer
		 * \param indices Pointer to the beginning of the indices buffer
		 * \param indexCount Number of indices in the buffer
		 * \return New index buffer
		 */
		static IndexBuffer* Create(const u32* indices, u32 indexCount);
		virtual ~IndexBuffer() = default;

		/**
		 * \brief Sets this buffer as the one to use for drawing operations
		 * \param target Target to bind to
		 */
		virtual void Bind(Surface* target) const = 0;
		/**
		 * \brief Draws the buffer using the bound buffers
		 * \param target Target to draw to
		 */
		virtual void Draw(Surface* target) const = 0;

	protected:
		static IndexBuffer* (*sCreateFunc)(const u32*, u32);
	};
}
