#include "Application.h"

EditorApp::EditorApp()
	: Application({})
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