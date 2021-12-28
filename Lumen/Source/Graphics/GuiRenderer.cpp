#include "GuiRenderer.h"

#include "Rhi/Gui.h"
#include "Rhi/Surface.h"

namespace Lumen::Graphics
{
	GuiRenderer::GuiRenderer(Window* target, Surface* surface, RenderPass* scene)
		: mSurface{ surface }, mGui{ Gui::Create() }
	{
		mGui->Init(target, mSurface);
		mGui->SetViewportImages(scene);
	}

	GuiRenderer::~GuiRenderer()
	{
		delete mGui;
	}

	void GuiRenderer::Render()
	{
		mGui->Begin();

		mGui->End();
	}
}