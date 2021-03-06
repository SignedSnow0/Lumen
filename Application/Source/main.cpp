#include "Application.h"

void Start()
{
	EditorApp app{};
	app.Init();
	app.Run();
	app.Shutdown();
}

#ifdef _DEBUG
#include <crtdbg.h>

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Start();

	return 0;
}
#else
#include <Windows.h>

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	Start();

	return 0;
}
#endif