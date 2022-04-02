#pragma once
#include <imgui/imgui.h>

#include "RenderPass.h"
#include "Surface.h"
#include "Platform/Window.h"

namespace Lumen::Graphics
{
	class Gui
	{
	public:
		Gui() = delete;
		Gui(const Gui&) = delete;
		Gui(Gui&&) = delete;
		Gui& operator=(const Gui&) = delete;
		Gui& operator=(Gui&&) = delete;
		
		/**
		 * \brief Creates a new Gui instance
		 * \return The new Gui instance
		 */
		static Gui* Create();
		virtual ~Gui() = default;

		/**
		 * \brief Returns the gui bold font
		 */
		virtual ImFont* GetBoldFont() = 0;
		/**
		 * \brief Returns the gui italic font
		 */
		virtual ImFont* GetItalicFont() = 0;

		/**
		 * \brief Initializes the gui resources
		 * \param target The target to render to
		 * \param surface The surface to render to
		 */
		virtual void Init(const Window* target, Surface* surface) = 0;
		/**
		 * \brief Releases the gui resources
		 */
		virtual void Release() = 0;

		/**
		 * \brief Begins rendering operations for this target
		 */
		virtual void Begin() = 0;
		/**
		 * \brief Ends rendering operations for this target
		 */
		virtual void End() = 0;
		/**
		 * \brief Sets the scene viewport images 
		 * \param scene The renderpass for the rendered scene
		 */
		virtual void SetViewportImages(RenderPass* scene) = 0;

	protected:
		static Gui* (*sCreateFunc)();
	};
}
