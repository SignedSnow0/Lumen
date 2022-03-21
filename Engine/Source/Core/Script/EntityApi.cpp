#include "EntityApi.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <mono/metadata/assembly.h>

#include "Core/Application.h"
#include "Core/Entity.h"

#define ENTITY_BASE "Lumen.Entity::"

#define BIND_FUNC(FName, ...) \
    mono_add_internal_call(ENTITY_BASE#FName"("#__VA_ARGS__")", (const void*)Script::EntityApi::FName);

namespace Lumen::Script
{
    void EntityApi::BindCalls()
    {
        mono_add_internal_call(ENTITY_BASE"CreateEntity", (const void*)Create);
        BIND_FUNC(Exists, uint)
        mono_add_internal_call(ENTITY_BASE"AddComponent", (const void*)AddComponent);
    }

    u32 EntityApi::Create()
    {
        return Application::Get()->GetScene()->CreateEntity().Id();
    }

    b8 EntityApi::Exists(u32 id)
    {
        for (const auto* e : Application::Get()->GetScene()->Entities())
        {
            if (e->Id() == id)
                return true;
        }
        return false;
    }

    void EntityApi::AddComponent(u32 id, MonoString* compName)
    {
        const char* comp = mono_string_to_utf8(compName);

        if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
        {
            if (strcmp(comp, "Transform") == 0)
                e.AddComponent<Components::Transform>();
            else if (strcmp(comp, "Tag") == 0)
                e.AddComponent<Components::Tag>();
        }
    }
}
