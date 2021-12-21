#include "EditorApplication.h"

EditorApp::EditorApp()
	: Application({ Lumen::Graphics::RendererAPI::Vulkan })
{

}

EditorApp::~EditorApp()
{

}

void EditorApp::Run()
{
	while (!Closing())
	{
		Application::Run();


	}
}