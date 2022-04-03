#pragma once
#include <vector>

#include "GraphicsTypes.h"
#include "Surface.h"
#include "Core/Types.h"

namespace Lumen::Graphics
{
	class RenderPass
	{
	public:
		/**
		 * @brief Creates a new RenderPass instance
		 * @param attachments Attachments to use in the render pass
		 * @param target Surface to draw to (if 0 the render pass will create its own texture as a target)
		 * @return The new RenderPass instance
		 */
		static RenderPass* Create(const std::vector<Attachment>& attachments, Surface* target = nullptr);
		virtual ~RenderPass() = default;

		/**
		 * @brief Initializes the renderpass resources
		 */
		virtual void Init() = 0;
		/**
		 * @brief Releases the renderpass resources
		 */
		virtual void Release() = 0;

		/**
		 * @brief Begins drawing operations into this target
		 * @param frame The frame index to render to
		 * @param target The target surface to synchronize if it has been created with one
		 */
		virtual void Begin(u32 frame, Surface* target = nullptr) = 0;
		/**
		 * @brief Ends drawing operations into this target
		 * @param target The target surface to synchronize if it has been created with one
		 */
		virtual void End(Surface* target = nullptr) = 0;
		/**
		 * @brief Resizes the renderpass
		 * @param width The new width
		 * @param height The new height
		 */
		virtual void Resize(u32 width, u32 height) = 0;

	protected:
		static RenderPass* (*sCreateFunc)(const std::vector<Attachment>&, Surface*);
	};
}
