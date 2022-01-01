#include "EditorApplication.h"

#include "Controls/EntityView.h"
#include "Controls/ResourcesView.h"
#include "Controls/SceneView.h"
#include "Controls/ToolbarView.h"

EditorApp* EditorApp::sInstance = nullptr;

EditorApp::EditorApp()
	: Application({ "Test", R"(D:\Dev\Lumen\TestProject\)", Lumen::Graphics::RendererAPI::Vulkan })
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

		auto e = GetScene()->CreateEntity();
		e.AddComponent<Lumen::Components::Transform>();

		return true;
	}

	return false;
}


void EditorApp::Run()
{
	while (!Closing())
	{
		Begin();
		Application::Run();

		mGui->Begin();

		mToolbarView->Render();
		mSceneView->Render();
		mResourcesView->Render();
		mEntityView->Render();

		mGui->End();

		if (Lumen::Entity e; GetScene()->GetEntity(0, e))
		{
			
		}

		End();
	}
}