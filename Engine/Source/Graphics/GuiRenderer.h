#pragma once
#include "Rhi/Gui.h"

struct ImFont;

namespace Lumen
{
	class Window;
}

namespace Lumen::Graphics
{
	class Gui;
	class DescriptorSet;
	class Pipeline;
	class Shader;
	class RenderPass;
	class Surface;

	class GuiRenderer
	{
	public:
		explicit GuiRenderer(Window* target, Surface* surface, RenderPass* scene);
		~GuiRenderer();

		[[nodiscard]] ImFont* BoldFont() const { return mGui->BoldFont(); }
		[[nodiscard]] ImFont* ItalicFont() const { return mGui->ItalicFont(); }

		void Begin();
		void End();

	private:
		Surface* mSurface{ nullptr };
		Gui* mGui{ nullptr };
	};
}
