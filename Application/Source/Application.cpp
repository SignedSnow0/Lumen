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
	while (!GetClosing())
	{
		Application::Run();


	}
}