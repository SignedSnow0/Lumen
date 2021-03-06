#include "EntityApi.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <mono/metadata/assembly.h>

#include "Core/Application.h"
#include "Core/Entity.h"

namespace Lumen::Script
{
#define ENTITY_BASE "Lumen.Entity::"

#define BIND_FUNC_ARGS(FName, ...) \
mono_add_internal_call(ENTITY_BASE#FName"("#__VA_ARGS__")", reinterpret_cast<const void*>(Script::EntityApi::FName))

#define BIND_FUNC(FName) \
mono_add_internal_call(ENTITY_BASE#FName, (const void*)Script::EntityApi::FName)

	void EntityApi::BindCalls()
	{
		mono_add_internal_call(ENTITY_BASE"CreateEntity", reinterpret_cast<const void*>(Create));
		BIND_FUNC_ARGS(Exists, uint);
		BIND_FUNC(AddComponent);
		BIND_FUNC(HasComponent);
	}

	u32 EntityApi::Create()
	{
		return Application::Get()->GetScene()->CreateEntity().GetId();
	}

	b8 EntityApi::Exists(const u32 id)
	{
		for (const auto* e : Application::Get()->GetScene()->GetEntities())
		{
			if (e->GetId() == id)
				return true;
		}
		return false;
	}

	void EntityApi::AddComponent(const u32 id, MonoString* compName)
	{
		const char* comp{ mono_string_to_utf8(compName) };

		if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
		{
			if (strcmp(comp, "Transform") == 0)
				e.AddComponent<Components::Transform>();
			else if (strcmp(comp, "Tag") == 0)
				e.AddComponent<Components::Tag>();
		}
	}

	b8 EntityApi::HasComponent(const u32 id, MonoString* compName)
	{
		const char* comp{ mono_string_to_utf8(compName) };

		if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
		{
			if (strcmp(comp, "Transform") == 0)
				return e.HasComponents<Components::Transform>();
			else if (strcmp(comp, "Tag") == 0)
				return e.HasComponents<Components::Tag>();
		}
		return false;
	}
}
