#pragma once
#include <Lumen.h>

class EditorApp : public Lumen::Application
{
public:
	EditorApp();
	~EditorApp() override;

	void Run() override;
};