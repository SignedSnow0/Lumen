#include "GuiRenderer.h"

#include "Rhi/Gui.h"
#include "Rhi/Surface.h"
#include "Utils/Logger.h"

namespace Lumen::Graphics
{
	GuiRenderer::GuiRenderer(Window* target, Surface* surface, RenderPass* scene)
		: mSurface{ surface }, mGui{ Gui::Create() }
	{
		mGui->Init(target, mSurface);
		mGui->SetViewportImages(scene);

		LOG_TRACE("GuiRenderer created");
	}

	GuiRenderer::~GuiRenderer()
	{
		delete mGui;

		LOG_TRACE("GuiRenderer destroyed");
	}

	void GuiRenderer::Begin()
	{
		mGui->Begin();
	}

	void GuiRenderer::End()
	{
		mGui->End();
	}
}
