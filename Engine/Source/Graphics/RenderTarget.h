#pragma once
#include "Platform/Window.h"
#include "Rhi/Surface.h"

namespace Lumen::Graphics
{
	struct RenderTarget
	{
		~RenderTarget();

		Window* Window{ nullptr };
		Surface* Surface{ nullptr };
	};
}
