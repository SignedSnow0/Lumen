#pragma once
#include <filesystem>
#include <vector>

#include "Project.h"
#include "Scene.h"
#include "Types.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/Rhi/GraphicsContext.h"
#include "Script/ScriptManager.h"

namespace Lumen
{
	namespace Graphics
	{
		class DefaultRenderer;
	}

	struct AppInitInfo
	{
		std::string Name{};
		std::filesystem::path Path{};
		Graphics::RendererAPI Api{};
	};

	class Application
	{
	public:
		explicit Application(const AppInitInfo& initInfo);
		virtual ~Application();

		[[nodiscard]] constexpr b8 Closing() const { return mShutdown; }
		[[nodiscard]] std::filesystem::path EnginePath() const { return mEnginePath; }
		[[nodiscard]] std::filesystem::path AssetsPath() const { return mEnginePath / "Assets"; }
		[[nodiscard]] Scene* GetScene() const { return mProject.Scene; }
		[[nodiscard]] const ScriptManager& GetScriptManager() const { return mScriptManager; }
		
		virtual b8 Init();

		virtual void Shutdown();
		virtual void Run();
		void Save();
		void Load();

		static Application* Get() { return sInstance; }

	protected:
		void Begin();
		void End();

		[[nodiscard]] constexpr Graphics::RenderTarget* GetRenderTarget(u32 index) { return &mWindows[index]; }
		[[nodiscard]] Graphics::DefaultRenderer* GetDefaultRenderer() const;

		Project								mProject{};
	private:
		b8									mShutdown{ false };
		std::filesystem::path				mEnginePath{ std::filesystem::current_path().parent_path() += "\\Engine\\" };
		std::vector<Graphics::RenderTarget>	mWindows{};
		Graphics::GraphicsContext*			mGraphicsContext{ nullptr };
		Graphics::SceneRenderer*			mRenderer{ nullptr };
		ScriptManager						mScriptManager{};
		
		static Application* sInstance;
	};
}
