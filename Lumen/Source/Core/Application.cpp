#include "Application.h"

#include "Window.h"
#include "Graphics/TestRenderer.h"

namespace Lumen
{
	Application::Application(AppInitInfo initInfo)
		: mInitInfo{ initInfo }
	{
		WindowInitInfo info
		{
			"Window",
			800,
			600
		};
		mWindows.emplace_back(info);
	}

	Application::~Application()
	{
		delete mGraphicsContext;

		mWindows.clear();
	}

	b8 Application::Initialize()
	{
		mShutdown = false;

		Graphics::GraphicsContext::SetRenderAPI(mInitInfo.Api);
		mGraphicsContext = Graphics::GraphicsContext::Create();
		mGraphicsContext->Init();

		Graphics::TestRenderer renderer{ &mWindows[0] };

		return true;
	}

	void Application::Shutdown()
	{

	}

	void Application::Run()
	{
		Window::Update();

		for(u32 i{ 0 }; i < mWindows.size(); i++)
		{
			if (mWindows[i].ShouldClose())
				mWindows.erase(mWindows.begin() + i);
		}
		if (mWindows.empty())
			mShutdown = true;

	}
}
