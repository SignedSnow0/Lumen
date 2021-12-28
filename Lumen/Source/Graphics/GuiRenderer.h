#pragma once

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

		void Render();

	private:
		Surface* mSurface{ nullptr };
		Gui* mGui{ nullptr };
	};
}
