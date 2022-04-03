#include "RenderTarget.h"

#include "Platform/Window.h"
#include "Rhi/Surface.h"

namespace Lumen::Graphics
{
	Graphics::RenderTarget::~RenderTarget()
	{
		delete Window;
		delete Surface;
	}
}
