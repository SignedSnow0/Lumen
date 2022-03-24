#include "ComponentsApi.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Application.h"
#include "Core/Entity.h"

#define COMPONENTS_BASE "Lumen.Components."

namespace Lumen::Script
{
    void ComponentsApi::BindCalls()
    {
        mono_add_internal_call(COMPONENTS_BASE"Transform::GetTranslation", (void*)GetTranslation);
        mono_add_internal_call(COMPONENTS_BASE"Transform::SetTranslation", (void*)SetTranslation);
        
        mono_add_internal_call(COMPONENTS_BASE"Transform::GetRotation", (void*)GetTranslation);
        mono_add_internal_call(COMPONENTS_BASE"Transform::SetRotation", (void*)SetRotation);
        
        mono_add_internal_call(COMPONENTS_BASE"Transform::GetScale", (void*)GetTranslation);
        mono_add_internal_call(COMPONENTS_BASE"Transform::SetScale", (void*)SetScale);
    }

    glm::vec3 ComponentsApi::GetTranslation(u32 id)
    {
        if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
        {
            const auto& vec{ e.GetComponents<Components::Transform>()->Translation };
                       
            return vec;
        }

        return glm::vec3{ 0.0f };
    }

    void ComponentsApi::SetTranslation(u32 id, float x, float y, float z)
    {
        if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
        {
            auto* transform = e.GetComponents<Components::Transform>();
            transform->Translation.x = x;
            transform->Translation.y = y;
            transform->Translation.z = z;
        }
    }

    glm::vec3 ComponentsApi::GetRotation(u32 id)
    {
        if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
        {
            const auto& vec{ e.GetComponents<Components::Transform>()->Rotation };
           
            return vec;
        }

        return glm::vec3{ 0.0f };
    }

    void ComponentsApi::SetRotation(u32 id, float x, float y, float z)
    {
        if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
        {
            auto* transform = e.GetComponents<Components::Transform>();
            transform->Rotation.x = x;
            transform->Rotation.y = y;
            transform->Rotation.z = z;
        }
    }

    glm::vec3 ComponentsApi::GetScale(u32 id)
    {
        if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
        {
            const auto& vec{ e.GetComponents<Components::Transform>()->Scale };
           
            return vec;
        }

        return glm::vec3{ 0.0f };
    }

    void ComponentsApi::SetScale(u32 id, float x, float y, float z)
    {
        if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
        {
            auto* transform = e.GetComponents<Components::Transform>();
            transform->Scale.x = x;
            transform->Scale.y = y;
            transform->Scale.z = z;
        }
    }
}
