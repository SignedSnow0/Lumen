#pragma once
#include <Lumen.h>
#pragma comment(lib, "Lumen.lib")

class EditorApp : public Lumen::Application
{
public:
	EditorApp();
	~EditorApp() override;

	void Run() override;
};