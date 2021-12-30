#include "ToolbarView.h"

#include "imgui/imgui.h"

void ToolbarView::Render()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Project"))
		{
			ImGui::MenuItem("Create");
			ImGui::MenuItem("Open");
			ImGui::MenuItem("Save");
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
