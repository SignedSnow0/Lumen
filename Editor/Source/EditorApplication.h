#pragma once
#include <filesystem>
#include <Lumen.h>

class ToolbarView;
class EntityView;
class ResourcesView;
class SceneView;

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
	SceneView* mSceneView{};
	ResourcesView* mResourcesView{};
	EntityView* mEntityView{};
	ToolbarView* mToolbarView{};
	std::filesystem::path mProjectPath{};

	Lumen::Graphics::GuiRenderer* mGui{ nullptr };

	static EditorApp* sInstance;
};
