#pragma once
#include <filesystem>
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
		[[nodiscard]] std::filesystem::path EnginePath() const { return mEnginePath; }
		[[nodiscard]] std::filesystem::path AssetsPath() const { return mEnginePath / "Assets"; }

		virtual b8 Init();

		virtual void Shutdown();
		virtual void Run();

		static Application* Get() { return sInstance; }

	protected:
		void Begin();
		void End();

		[[nodiscard]] constexpr RenderTarget* GetRenderTarget(u32 index) { return &mWindows[index]; }
		[[nodiscard]] constexpr Graphics::DefaultRenderer* GetDefaultRenderer() const { return renderer; }

	private:
		AppInitInfo mInitInfo{};
		b8 mShutdown{ false };
		std::filesystem::path mEnginePath{ std::filesystem::current_path().parent_path() += "\\Lumen\\" };

		std::vector<RenderTarget> mWindows{};
		Graphics::GraphicsContext* mGraphicsContext{ nullptr };

		Graphics::DefaultRenderer* renderer{ nullptr };

		static Application* sInstance;
	};
}
