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
		std::string Name{};
		std::filesystem::path Path{};
		Graphics::RendererApi Api{};
	};

	class Application
	{
	public:
		explicit Application(const AppInitInfo& initInfo);
		virtual ~Application();

		b8 GetClosing() const;
		std::filesystem::path GetEnginePath() const;
		std::filesystem::path GetAssetsPath() const;
		Scene* GetScene() const;
		Script::ScriptManager& GetScriptManager();
		b8 GetIsPlaying() const;
		void SetPlaying(b8 value);
		
		virtual b8 Init();
		virtual void Shutdown();
		
		virtual void Run();
		
		void Save();
		void Load();

		static Application* Get() { return sInstance; }

	protected:
		void Begin();
		void End();

		Graphics::RenderTarget* GetRenderTarget(u32 index);
		Graphics::DefaultRenderer* GetDefaultRenderer() const;

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
