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
			e.AddComponent<Components::Tag>();
		}

		u32 i{ 0 };
		for (const auto e : scene->GetEntities())
		{
			auto* name{ e->GetComponents<Components::Tag>() };
			std::string label{ "Entity##" };
			if (name)
				label = name->Name + "##" + std::to_string(i);
			else
				label += std::to_string(i);
			
			if (ImGui::Selectable(label.c_str(), mSelectedEntity ? mSelectedEntity->GetId() == e->GetId() : false))
				mSelectedEntity = e;
			
			i++;
		}
	}
	ImGui::End();
}
