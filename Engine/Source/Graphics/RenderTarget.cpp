#include "RenderTarget.h"

#include "Core/Window.h"
#include "Rhi/Surface.h"

namespace Lumen::Graphics
{
	Graphics::RenderTarget::~RenderTarget()
	{
		delete Window;
		delete Surface;
	}
}
