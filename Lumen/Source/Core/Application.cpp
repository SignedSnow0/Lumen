#include "Application.h"

#include "Window.h"
#include "Graphics/Rhi/Surface.h"
#include "Utils/Serializer.h"
#include "Graphics/SceneRenderer.h"

namespace Lumen
{
	Application* Application::sInstance = nullptr;

	Application::Application(const AppInitInfo& initInfo)
	{
		mProject.Name = initInfo.Name;
		mProject.Path = initInfo.Path;
		mProject.Api = initInfo.Api;

		constexpr WindowInitInfo info
		{
			"Window",
			1920,
			1080
		};

		Graphics::RenderTarget renderTarget{};
		mWindows.emplace_back(new Window{ info }, nullptr);

		sInstance = this;
	}

	Application::~Application()
	{
		delete mGraphicsContext;

		mWindows.clear();
	}

	b8 Application::Init()
	{
		mShutdown = false;

		Graphics::GraphicsContext::SetRenderAPI(mProject.Api);
		mGraphicsContext = Graphics::GraphicsContext::Create();
		mGraphicsContext->Init();

		for (auto& renderTarget : mWindows)
		{
			renderTarget.Surface = Graphics::Surface::Create(renderTarget.Window);
			renderTarget.Surface->Init();
		}
		
		mProject.Scene = new Scene{};

		mRenderer = new Graphics::SceneRenderer{ mProject.Scene };
		
		return true;
	}

	void Application::Shutdown()
	{
		Graphics::GraphicsContext::Get().WaitIdle();
		
		delete mRenderer;
	}

	void Application::Run()
	{
	}

	void Application::Begin()
	{
		Window::Update();

		mWindows[0].Surface->Begin();
	}

	void Application::End()
	{
		mRenderer->Render(mWindows[0]);

		mWindows[0].Surface->End();

		for (u32 i{ 0 }; i < mWindows.size(); i++)
		{
			if (mWindows[i].Window->ShouldClose())
			{
				mGraphicsContext->WaitIdle();
				mWindows.erase(mWindows.begin() + i);
			}
		}
		if (mWindows.empty())
			mShutdown = true;
	}

	void Application::Save()
	{
		Utils::Serializer serializer{ &mProject };
		serializer.Save(mProject.Path / (mProject.Name + ".lumen"));
	}

	void Application::Load()
	{
		
	}

	Graphics::DefaultRenderer* Application::GetDefaultRenderer() const
	{
		return mRenderer->GetDefaultRenderer();
	}
}
