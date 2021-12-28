#pragma once
#include <Lumen.h>

class EditorApp : public Lumen::Application
{
public:
	EditorApp();
	~EditorApp() override;

	b8 Init() override;

	void Run() override;

private:
	Lumen::Graphics::GuiRenderer* mGui{ nullptr };
};