#pragma once
#include "Project.h"
#include "Scene.h"
#include "Graphics/DefaultRenderer.h"
#include "Graphics/RenderTarget.h"
#include "Script/ScriptManager.h"

namespace Lumen
{
	struct AppInitInfo
	{
		std::string AppName{};
		std::filesystem::path ProjectPath{};
		Graphics::RendererApi GraphicsApi{};
		WindowInitInfo WindowInitInfo{};
	};

	class Application
	{
	public:
		explicit Application(const AppInitInfo& initInfo);
		virtual ~Application();

		[[nodiscard]] std::filesystem::path GetEnginePath() const;
		[[nodiscard]] std::filesystem::path GetAssetsPath() const;
		[[nodiscard]] b8 GetClosing() const;
		[[nodiscard]] b8 GetIsPlaying() const;
		void SetPlaying(b8 value);
		[[nodiscard]] Scene* GetScene() const;
		[[nodiscard]] Script::ScriptManager& GetScriptManager();

		virtual b8 Init();
		virtual void Shutdown();
		virtual void Run();

		void Save();
		void Load();

		[[nodiscard]] static Application* Get() { return sInstance; }

	protected:
		void Begin();
		void End();

		[[nodiscard]] Graphics::RenderTarget* GetRenderTarget(u32 index);
		[[nodiscard]] Graphics::DefaultRenderer* GetDefaultRenderer() const;

		Project	mProject{};

	private:
		b8 mShutdown{ false };
		b8 mIsPlaying{ false };
		std::filesystem::path mEnginePath{ std::filesystem::current_path().parent_path() += "\\Engine\\" };
		std::vector<Graphics::RenderTarget>	mWindows{};
		Graphics::GraphicsContext* mGraphicsContext{ nullptr };
		Graphics::SceneRenderer* mRenderer{ nullptr };
		Script::ScriptManager mScriptManager{};

		static Application* sInstance;
	};
}
