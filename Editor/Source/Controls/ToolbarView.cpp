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

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Settings"))
		{

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
