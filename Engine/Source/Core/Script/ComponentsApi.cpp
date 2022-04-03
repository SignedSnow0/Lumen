#include "ComponentsApi.h"

#include <glm/gtc/type_ptr.hpp>

#include "Core/Application.h"
#include "Core/Entity.h"

namespace Lumen::Script
{
#define COMPONENTS_BASE "Lumen.Components."

	void ComponentsApi::BindCalls()
	{
		mono_add_internal_call(COMPONENTS_BASE"Transform::GetTranslation", reinterpret_cast<void*>(GetTranslation));
		mono_add_internal_call(COMPONENTS_BASE"Transform::SetTranslation", reinterpret_cast<void*>(SetTranslation));

		mono_add_internal_call(COMPONENTS_BASE"Transform::GetRotation", reinterpret_cast<void*>(GetRotation));
		mono_add_internal_call(COMPONENTS_BASE"Transform::SetRotation", reinterpret_cast<void*>(SetRotation));

		mono_add_internal_call(COMPONENTS_BASE"Transform::GetScale", reinterpret_cast<void*>(GetScale));
		mono_add_internal_call(COMPONENTS_BASE"Transform::SetScale", reinterpret_cast<void*>(SetScale));
	}

	glm::vec3 ComponentsApi::GetTranslation(const u32 id)
	{
		if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
		{
			const auto& vec{ e.GetComponents<Components::Transform>()->Translation };
			return vec;
		}

		return glm::vec3{ 0.0f };
	}

	void ComponentsApi::SetTranslation(const u32 id, const float x, const float y, const float z)
	{
		if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
		{
			auto* transform = e.GetComponents<Components::Transform>();
			transform->Translation.x = x;
			transform->Translation.y = y;
			transform->Translation.z = z;
		}
	}

	glm::vec3 ComponentsApi::GetRotation(const u32 id)
	{
		if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
		{
			const auto& vec{ e.GetComponents<Components::Transform>()->Rotation };
			return vec;
		}

		return glm::vec3{ 0.0f };
	}

	void ComponentsApi::SetRotation(const u32 id, const float x, const float y, const float z)
	{
		if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
		{
			auto* transform = e.GetComponents<Components::Transform>();
			transform->Rotation.x = x;
			transform->Rotation.y = y;
			transform->Rotation.z = z;
		}
	}

	glm::vec3 ComponentsApi::GetScale(const u32 id)
	{
		if (Entity e; Application::Get()->GetScene()->GetEntity(id, e))
		{
			const auto& vec{ e.GetComponents<Components::Transform>()->Scale };
			return vec;
		}

		return glm::vec3{ 0.0f };
	}

	void ComponentsApi::SetScale(const u32 id, const float x, const float y, const float z)
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
