#include "SceneView.h"

#include <imgui/imgui.h>

using namespace Lumen;

void SceneView::Render()
{
	if (ImGui::Begin("Scene##View"))
	{
		Scene* scene{ Application::Get()->GetScene() };
		
		if (ImGui::Button("Create entity"))
		{
			auto e{ scene->CreateEntity() };
			e.AddComponent<Components::Transform>();
		}

		u32 i{ 0 };
		for (const auto e : scene->Entities())
		{
			std::string label{"Entity "};
			label.append(std::to_string(i));
			if (ImGui::Selectable(label.c_str(), mSelectedEntity ? mSelectedEntity->Id() == e->Id() : false))
				mSelectedEntity = e;
			i++;
		}
	}
	ImGui::End();
}
