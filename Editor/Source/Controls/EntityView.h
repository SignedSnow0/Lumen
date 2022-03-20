#pragma once
#include "SceneView.h"

class EntityView
{
public:
	EntityView(SceneView* sceneView);
	~EntityView() = default;

	void Render();

private:
	SceneView* mSceneView{};
};
