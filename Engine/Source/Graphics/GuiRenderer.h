#pragma once
#include "Rhi/Gui.h"

namespace Lumen::Graphics
{
	class GuiRenderer
	{
	public:
		explicit GuiRenderer(Window* target, Surface* surface, RenderPass* scene);
		~GuiRenderer();

		[[nodiscard]] ImFont* BoldFont() const { return mGui->GetBoldFont(); }
		[[nodiscard]] ImFont* ItalicFont() const { return mGui->GetItalicFont(); }

		void Begin();
		void End();

	private:
		Surface* mSurface{ nullptr };
		Gui* mGui{ nullptr };
	};
}
