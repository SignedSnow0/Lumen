#include "ComponentsApi.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/Application.h"
#include "Core/Entity.h"

#define COMPONENTS_BASE "Lumen.Components."

namespace Lumen::Script
{
    void ComponentsApi::BindCalls()
    {
        mono_add_internal_call(COMPONENTS_BASE"Transform::GetTranslation", (const void*)GetTranslation);
        mono_add_internal_call(COMPONENTS_BASE"Transform::SetTranslation", (const void*)SetTranslation);
        
        mono_add_internal_call(COMPONENTS_BASE"Transform::GetRotation", (const void*)GetTranslation);
        mono_add_internal_call(COMPONENTS_BASE"Transform::SetRotation", (const void*)SetRotation);
        
        mono_add_internal_call(COMPONENTS_BASE"Transform::GetScale", (const void*)GetTranslation);
        mono_add_internal_call(COMPONENTS_BASE"Transform::SetScale", (const void*)SetScale);
    }

    void* ComponentsApi::GetTranslation(u32 id)
    {
        if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
        {
            return glm::value_ptr(e.GetComponents<Components::Transform>()->Translation);
        }
        
        glm::vec3 val{0.0f};
        return glm::value_ptr(val);
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

    void* ComponentsApi::GetRotation(u32 id)
    {
        if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
        {
            return glm::value_ptr(e.GetComponents<Components::Transform>()->Rotation);
        }

        glm::vec3 val{0.0f};
        return glm::value_ptr(val);
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

    void* ComponentsApi::GetScale(u32 id)
    {
        if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
        {
            return glm::value_ptr(e.GetComponents<Components::Transform>()->Scale);
        }
        
        glm::vec3 val{0.0f};
        return glm::value_ptr(val);
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
