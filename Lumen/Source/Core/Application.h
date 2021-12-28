#pragma once
#include <vector>

#include "Types.h"
#include "Window.h"
#include "Graphics/Rhi/GraphicsContext.h"

namespace Lumen
{
	namespace Graphics
	{
		class Surface;
		class GuiRenderer;
		class DefaultRenderer;
	}

	struct AppInitInfo
	{
		Graphics::RendererAPI Api{};
	};

	struct RenderTarget
	{
		~RenderTarget();

		Window* Window{ nullptr };
		Graphics::Surface* Surface{ nullptr };
	};

	class Application
	{
	public:
		explicit Application(AppInitInfo initInfo);
		virtual ~Application();

		[[nodiscard]] constexpr b8 Closing() const { return mShutdown; }

		virtual b8 Init();

		virtual void Shutdown();
		virtual void Run();

	protected:
		void Begin();
		void End();

		[[nodiscard]] constexpr RenderTarget* GetRenderTarget(u32 index) { return &mWindows[index]; }
		[[nodiscard]] constexpr Graphics::DefaultRenderer* GetDefaultRenderer() const { return renderer; }

	private:
		AppInitInfo mInitInfo{};
		b8 mShutdown{ false };
	
		std::vector<RenderTarget> mWindows{};
		Graphics::GraphicsContext* mGraphicsContext{ nullptr };

		Graphics::DefaultRenderer* renderer{ nullptr };
	};
}
