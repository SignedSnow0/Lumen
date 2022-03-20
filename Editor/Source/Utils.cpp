#include "Utils.h"
#include <imgui/imgui.h>

#include "imgui/imgui_internal.h"

b8 DrawVec3(const std::string& label, glm::vec3& vec, const glm::vec3& defaultValue)
{
    b8 modified{ false };

    ImGui::Text(label.c_str());
    ImGui::PushID(label.c_str());
    ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, ImGui::GetCurrentContext()->Style.ItemSpacing.y });
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

    const f32 lineHeight{ ImGui::GetIO().FontDefault->FontSize + ImGui::GetCurrentContext()->Style.FramePadding.y * 2.0f };
    const ImVec2 buttonSize{ lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
    if (ImGui::Button("X", buttonSize))
    {
        vec.x = defaultValue.x;
        modified = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    modified |= ImGui::DragFloat("##X", &vec.x, 0.1f, 0, 0, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.0f });
    if (ImGui::Button("Y", buttonSize))
    {
        vec.y = defaultValue.y;
        modified = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    modified |= ImGui::DragFloat("##Y", &vec.y, 0.1f, 0, 0, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.1f, 0.25f, 0.8f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.1f, 0.25f, 0.8f, 1.0f });
    if (ImGui::Button("Z", buttonSize))
    {
        vec.z = defaultValue.z;
        modified = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    modified |= ImGui::DragFloat("##Z", &vec.z, 0.1f, 0, 0, "%.2f");
    ImGui::PopItemWidth();
    
    ImGui::PopStyleVar(2);
    ImGui::PopID();
    
    return modified;
}

b8 DrawVec2(const std::string& label, glm::vec2& vec, const glm::vec2& defaultValue)
{
     b8 modified{ false };

    ImGui::Text(label.c_str());
    ImGui::PushID(label.c_str());
    ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, ImGui::GetCurrentContext()->Style.ItemSpacing.y });
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);

    const f32 lineHeight{ ImGui::GetIO().FontDefault->FontSize + ImGui::GetCurrentContext()->Style.FramePadding.y * 2.0f };
    const ImVec2 buttonSize{ lineHeight + 3.0f, lineHeight };

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.8f, 0.1f, 0.15f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.8f, 0.1f, 0.15f, 1.0f });
    if (ImGui::Button("X", buttonSize))
    {
        vec.x = defaultValue.x;
        modified = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    modified |= ImGui::DragFloat("##X", &vec.x, 0.1f, 0, 0, "%.2f");
    ImGui::PopItemWidth();
    ImGui::SameLine();

    ImGui::PushStyleColor(ImGuiCol_Button, { 0.2f, 0.7f, 0.2f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.2f, 0.7f, 0.2f, 1.0f });
    if (ImGui::Button("Y", buttonSize))
    {
        vec.y = defaultValue.y;
        modified = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::SameLine();
    modified |= ImGui::DragFloat("##Y", &vec.y, 0.1f, 0, 0, "%.2f");
    ImGui::PopItemWidth();

    ImGui::PopStyleVar(2);
    ImGui::PopID();
    
    return modified;
}
