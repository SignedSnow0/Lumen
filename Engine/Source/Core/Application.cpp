#include "Application.h"
#include "Platform/Window.h"
#include "Graphics/SceneRenderer.h"
#include "Utils/Logger.h"
#include "Utils/Serializer.h"

namespace Lumen
{
	/**
	 * \brief Singleton instance for this class
	 */
	Application* Application::sInstance = nullptr;

	/**
	 * \brief Creates a new instance of the Application class
	 * \param initInfo Info about the apps initial settings
	 */
	Application::Application(const AppInitInfo& initInfo)
	{
		//fill out the project info
		mProject.Name = initInfo.AppName;
		mProject.Path = initInfo.ProjectPath;
		mProject.Api = initInfo.GraphicsApi;

		//creates a new rt for the initial window
		Graphics::RenderTarget renderTarget{};
		mWindows.emplace_back(new Window{ initInfo.WindowInitInfo }, nullptr);

		sInstance = this;
	}

	Application::~Application()
	{
		delete mGraphicsContext;

		mWindows.clear();
	}

	/**
	 * \brief Returns true if no window is open and the app is going 
	 */
	b8 Application::GetClosing() const { return mShutdown; }

	/**
	 * \brief Returns the folder of the engine project
	 */
	std::filesystem::path Application::GetEnginePath() const { return mEnginePath; }

	/**
	 * \brief Returns the folder of the engines assets path
	 */
	std::filesystem::path Application::GetAssetsPath() const { return mEnginePath / "Assets"; }

	/**
	 * \brief Returns the current scene if present
	 */
	Scene* Application::GetScene() const { return mProject.Scene; }

	/**
	 * \brief Returns the script manager
	 */
	Script::ScriptManager& Application::GetScriptManager() { return mScriptManager; }

	/**
	 * \brief Returns true is the scene is on playing mode
	 */
	b8 Application::GetIsPlaying() const { return mIsPlaying; }

	/**
	 * \brief If set to true starts the scene playing mode
	 */
	void Application::SetPlaying(const b8 value) { mIsPlaying = value; } 

	
	/**
	 * \brief Initializes the applications resources
	 * \return True if initialization is successful
	 */
	b8 Application::Init()
	{
		mShutdown = false;

		Utils::Logger::SetLogLevel(Utils::LogLevel::Trace);

		//initialize graphics system
		Graphics::GraphicsContext::SetRenderApi(mProject.Api);
		mGraphicsContext = Graphics::GraphicsContext::Create();
		mGraphicsContext->Init();

		//finishes initializations of all the rts created before inialization 
		for (auto& [window, surface] : mWindows)
		{
			surface = Graphics::Surface::Create(window);
			surface->Init();
		}

		//creates a new scene
		mProject.Scene = new Scene{};
		//and a renderer for the scene
		mRenderer = new Graphics::SceneRenderer{ mProject.Scene };

		//if the project already exists loads the scene
		if (std::filesystem::exists(mProject.Path))
			Load();

		//initializes c# scripting
		mScriptManager.Init();

		LOG_INFO("Application initialized");

		return true;
	}

	/**
	 * \brief Shuts down the application
	 */
	void Application::Shutdown()
	{
		mScriptManager.Shutdown();
		
		Graphics::GraphicsContext::Get().WaitIdle();
		
		delete mRenderer;

		LOG_INFO("Application shutdown");
	}

	void Application::Run()
	{
	}

	/**
	 * \brief Begins drawing operations for the window
	 */
	void Application::Begin()
	{
		//get platforms updates
		Window::Update();

		mWindows[0].Surface->Begin();

		//if scene is in playing mode updates the scene before rendering
		if (mIsPlaying)
		{
			mScriptManager.Update();
		}
		
	}

	/**
	 * \brief Ends drawing operations for the window
	 */
	void Application::End()
	{
		//submits rendering operations to the gpu
		mRenderer->Render(mWindows[0]);
		mWindows[0].Surface->End();

		//erases windows that are closed
		for (u32 i{ 0 }; i < mWindows.size(); i++)
		{
			if (mWindows[i].Window->ShouldClose())
			{
				mGraphicsContext->WaitIdle();
				mWindows.erase(mWindows.begin() + i);
			}
		}
		//if no window is left closes the application
		if (mWindows.empty())
			mShutdown = true;
	}

	/**
	 * \brief Returns the RenderTarget of the window
	 * \param index Index of the window
	 */
	Graphics::RenderTarget* Application::GetRenderTarget(const u32 index) { return &mWindows[index]; }

	/**
	 * \brief Saves the current project of the application
	 */
	void Application::Save()
	{
		Utils::Serializer serializer{ &mProject };
		serializer.Save(mProject.Path / (mProject.Name + ".lumen"));
	}

	/**
	 * \brief Loads the current project
	 */
	void Application::Load()
	{
		Utils::Serializer serializer{ &mProject };
		serializer.Load( mProject.Path / (mProject.Name + ".lumen"));
	}

	/**
	 * \brief Returns the applications default renderer
	 */
	Graphics::DefaultRenderer* Application::GetDefaultRenderer() const
	{
		return mRenderer->GetDefaultRenderer();
	}
}
