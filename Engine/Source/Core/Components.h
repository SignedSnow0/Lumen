#pragma once
#include "Types.h"

#include <string>
#include <glm/glm.hpp>

namespace Lumen::Components
{
	enum class ComponentType : u32
	{
		Transform,
		Mesh
	};

	struct Component
	{

	};

	struct Transform : Component
	{
		glm::mat4 GetTransform() const;

		glm::vec3 Translation{ 0.0f };
		glm::vec3 Rotation{ 0.0f };
		glm::vec3 Scale{ 1.0f };

		inline static const char* ComponentName = "Transform";
	};

	struct Tag : Component
	{
		std::string Name{ "Empty entity" };

		inline static const char* ComponentName = "Tag";
	};

	struct Mesh : Component
	{
		inline static const char* ComponentName = "Mesh";
	};

	struct Script : Component
	{
		std::string ScriptName{ "Script" };
		
		inline static const char* ComponentName = "Script";
	};
}
