#include "EditorApplication.h"

EditorApp* EditorApp::sInstance = nullptr;

EditorApp::EditorApp()
	: Application({ "Test", R"(C:\Dev\Lumen\TestProject\)", Lumen::Graphics::RendererApi::Vulkan })
{
	sInstance = this;
}

EditorApp::~EditorApp()
{
	delete mGui;

}

b8 EditorApp::Init()
{
	if (Application::Init())
	{
		mGui = new Lumen::Graphics::GuiRenderer{ GetRenderTarget(0)->Window, GetRenderTarget(0)->Surface, GetDefaultRenderer()->GetRenderPass() };

		return true;
	}

	return false;
}


void EditorApp::Run()
{
	while (!GetClosing())
	{
		Begin();
		Application::Run();

		mGui->Begin();

		mToolbarView.Render();
		mSceneView.Render();
		mResourcesView.Render();
		mEntityView.Render();

		mGui->End();

		if (Lumen::Entity e; GetScene()->GetEntity(0, e))
		{
			
		}

		End();
	}
}