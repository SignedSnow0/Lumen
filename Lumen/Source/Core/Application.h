#pragma once
#include <vector>

#include "Types.h"
#include "Window.h"
#include "Graphics/Rhi/GraphicsContext.h"

namespace Lumen
{
	struct AppInitInfo
	{
		Graphics::RendererAPI Api{};
	};

	class Application
	{
	public:
		explicit Application(AppInitInfo initInfo);
		virtual ~Application();

		[[nodiscard]] constexpr b8 Closing() const { return mShutdown; }

		virtual b8 Initialize();
		virtual void Shutdown();
		virtual void Run();

	private:
		AppInitInfo mInitInfo{};
		b8 mShutdown{ false };
		std::vector<Window> mWindows{};
		Graphics::GraphicsContext* mGraphicsContext{ nullptr };
	};
}
