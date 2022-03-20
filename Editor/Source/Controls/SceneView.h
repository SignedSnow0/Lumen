#pragma once
#include <Lumen.h>

class SceneView
{
public:
	SceneView() = default;
	~SceneView() = default;

	void Render();

	[[nodiscard]] Lumen::Entity* SelectedEntity() { return mSelectedEntity; }

private:
	Lumen::Entity* mSelectedEntity{ nullptr };
};