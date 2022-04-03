#pragma once
#include "Core/Types.h"
#include "Platform/Window.h"

namespace Lumen::Graphics
{
	class Surface
	{
	public:
		/**
		 * @brief Creates a new Surface instance
		 * @param target Window the surface will be created for
		 * @return The new Surface instance
		 */
		static Surface* Create(Window* target);
		virtual ~Surface() = default;

		/**
		 * @brief Initializes the surface resources
		 */
		virtual void Init() = 0;
		/**
		 * @brief Releases the surface resources
		 */
		virtual void Release() = 0;

		/**
		 * @brief Returns the frame index on which the app is currently rendering
		 */
		virtual u32 CurrentFrame() const = 0;

		/**
		 * @brief Begins rendering operations into this target
		 */
		virtual void Begin() = 0;
		/**
		 * @brief Ends rendering operations into this target
		 */
		virtual	void End() = 0;

	protected:
		static Surface* (*sCreateFunc)(Window*);
	};
}
