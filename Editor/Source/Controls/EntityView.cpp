#include "EntityView.h"
#include <imgui/imgui.h>

#include "../EditorApplication.h"
#include "../Utils.h"

using namespace Lumen;

namespace
{
	void DrawTransform(Components::Transform& transform)
	{
		ImGui::Text("Transform component");
		ImGui::Indent();
		DrawVec3("Translation", transform.Translation);
		DrawVec3("Rotation", transform.Rotation);
		DrawVec3("Scale", transform.Scale, glm::vec3{ 1.0f });
		ImGui::Unindent();
	}
}

EntityView::EntityView(SceneView* sceneView)
	: mSceneView{ sceneView }
{
}

void EntityView::Render()
{
	if (ImGui::Begin("Entity##View"))
	{
		if (const auto e = mSceneView->SelectedEntity(); e)
		{
			if (ImGui::BeginMenu("Add component"))
			{
				if (ImGui::MenuItem(Components::Script::ComponentName))
					e->AddComponent<Components::Script>();
				
				ImGui::EndMenu();
			}
			
			if (auto* tag = e->GetComponents<Components::Tag>(); tag)
			{
				ImGui::Text("Name: ");
				ImGui::SameLine();
				char buf[256]{ 0 };
				strcpy_s(buf, 256, tag->Name.c_str());
				ImGui::InputText("##TagComponent", buf, 256);
				tag->Name = buf;
			}
			
			if (auto* transform = e->GetComponents<Components::Transform>(); transform)
			{
				DrawTransform(*transform);
			}

			if (auto* script = e->GetComponents<Components::Script>(); script)
			{
				ImGui::Text("Script component");
				ImGui::Indent();

				auto& scripts{ EditorApp::Get()->GetScriptManager().Scripts() };
				ImGui::Text("Script class: ");
				ImGui::SameLine();
				if (ImGui::BeginMenu(script->ScriptName.c_str()))
				{
					for (auto& [name, _] : scripts)
					{
						if (ImGui::MenuItem(name.c_str()))
						{
							script->ScriptName = name;	
						}
					}

					ImGui::EndMenu();
				}
				
				ImGui::Unindent();
			}
		}
	}
	ImGui::End();
}
