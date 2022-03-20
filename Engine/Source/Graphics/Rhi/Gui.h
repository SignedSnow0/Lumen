#pragma once

struct ImFont;

namespace Lumen
{
	class Window;
}

namespace Lumen::Graphics
{
	class RenderPass;
	class Surface;

	class Gui
	{
	public:
		static Gui* Create();
		virtual ~Gui() = default;

		virtual ImFont* BoldFont() = 0;
		virtual ImFont* ItalicFont() = 0;

		virtual void Init(const Window* target, Surface* surface) = 0;
		virtual void Release() = 0;

		virtual void Begin() = 0;
		virtual void End() = 0;
		virtual void SetViewportImages(RenderPass* scene) = 0;

	protected:
		static Gui* (*sCreateFunc)();
	};
}
