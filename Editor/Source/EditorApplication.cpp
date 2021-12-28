#include "EditorApplication.h"

EditorApp::EditorApp()
	:	Application({ Lumen::Graphics::RendererAPI::Vulkan })
{
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
	while (!Closing())
	{
		Begin();

		Application::Run();

		mGui->Render();

		End();
	}
}