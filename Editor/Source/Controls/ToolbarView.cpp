#include "ToolbarView.h"

#include "../EditorApplication.h"

#include <imgui/imgui.h>

void ToolbarView::Render()
{	
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Project"))
		{
			if (ImGui::MenuItem("Create"))
			{
				mFileOpen = true;
			}
			
			ImGui::MenuItem("Open");
			if (ImGui::MenuItem("Save"))
			{
				EditorApp::Get()->Save();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Actions"))
		{
			const b8 scenePlaying{ EditorApp::Get()->GetIsPlaying() };
			if (ImGui::MenuItem(scenePlaying ? "Stop" : "Play"))
			{
				static const char* tempFile{ "$Temp$.lumen" };
				EditorApp::Get()->SetPlaying(!scenePlaying);

				auto& project{ EditorApp::Get()->GetProject() };
				Lumen::Utils::Serializer serializer{ &project };
				scenePlaying ? serializer.Load(project.Path / tempFile) : serializer.Save(project.Path / tempFile);
				if (scenePlaying)
				{
					EditorApp::Get()->GetSceneView().mSelectedEntity = nullptr;
					std::filesystem::remove(project.Path / tempFile);
				}
				else
				{
					EditorApp::Get()->GetScriptManager().Start(EditorApp::Get()->GetScene());
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

	CreateDialog();
}


void ToolbarView::CreateDialog()
{
	if (mFileOpen)
	{
		ImGui::OpenPopup("Select a location");
		mFileOpen = false;
		if (!std::filesystem::exists(mFileExplorer.GetCurrentPath()))
		{
			mFileExplorer.SetCurrentPath("C:\\");
		}
	}

	if (ImGui::BeginPopupModal("Select a location"))
	{
		mFileExplorer.Draw();
		
		if (ImGui::Button("Create"))
			ImGui::CloseCurrentPopup();
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			ImGui::CloseCurrentPopup();
		
		ImGui::EndPopup();
	}
}
