#pragma once
#include <filesystem>
#include <Lumen.h>

#include "Controls/EntityView.h"
#include "Controls/ResourcesView.h"
#include "Controls/SceneView.h"
#include "Controls/ToolbarView.h"

class EditorApp : public Lumen::Application
{
public:
	EditorApp();
	~EditorApp() override;

	[[nodiscard]] constexpr Lumen::Graphics::GuiRenderer* GuiRenderer() const { return mGui; }

	b8 Init() override;

	void Run() override;

	static EditorApp* Get() { return sInstance; }

private:
	EntityView mEntityView{ &mSceneView };
	SceneView mSceneView{};
	ResourcesView mResourcesView{};
	ToolbarView mToolbarView{};
	std::filesystem::path mProjectPath{};

	Lumen::Graphics::GuiRenderer* mGui{ nullptr };

	static EditorApp* sInstance;
};
