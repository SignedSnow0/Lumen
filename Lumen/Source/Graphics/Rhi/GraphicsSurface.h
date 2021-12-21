#pragma once

namespace Lumen
{
	class Window;
}

namespace Lumen::Graphics
{
	class GraphicsSurface
	{
	public:
		explicit GraphicsSurface(const Window& window);

	protected:
		Window* mWindow{ nullptr };
	};
}
