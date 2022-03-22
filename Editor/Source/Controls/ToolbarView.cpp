#include "ToolbarView.h"

#include "../EditorApplication.h"

#include <imgui/imgui.h>

void ToolbarView::Render()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Project"))
		{
			ImGui::MenuItem("Create");
			ImGui::MenuItem("Open");
			if(ImGui::MenuItem("Save"))
			{
				EditorApp::Get()->Save();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Actions"))
		{
			const b8 scenePlaying{ EditorApp::Get()->IsPlaying() };
			if (ImGui::MenuItem(scenePlaying ? "Stop" : "Play"))
			{
				static const char* tempFile{ "$Temp$.lumen" };
				EditorApp::Get()->SetPlaying(!scenePlaying);

				auto& project{ EditorApp::Get()->GetProject() };
				Lumen::Utils::Serializer serializer{ &project };
				scenePlaying ? serializer.Load(project.Path / tempFile) : serializer.Save(project.Path / tempFile);
				if (-scenePlaying)
				{
					EditorApp::Get()->GetSceneView().mSelectedEntity = nullptr;
					std::filesystem::remove(project.Path / tempFile);
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Settings"))
		{
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
