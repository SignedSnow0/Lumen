#pragma once

namespace Lumen
{
	class Window;
}

namespace Lumen::Graphics
{
	class Surface;

	struct RenderTarget
	{
		~RenderTarget();

		Window* Window{ nullptr };
		Graphics::Surface* Surface{ nullptr };
	};
}
