﻿#include "Application.h"

#include "Window.h"
#include "Graphics/DefaultRenderer.h"
#include "Utils/Serializer.h"

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

		renderer = new Graphics::DefaultRenderer{};

		mProject.Scene = new Scene{};

		return true;
	}

	void Application::Shutdown()
	{
		Graphics::GraphicsContext::Get().WaitIdle();

		delete renderer;
	}

	void Application::Run()
	{
		renderer->Render(mWindows[0].Surface);
	}

	void Application::Begin()
	{
		Window::Update();

		mWindows[0].Surface->Begin();
	}

	void Application::End()
	{
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
}
