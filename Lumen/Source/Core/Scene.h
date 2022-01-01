#pragma once
#include <entt/entt.hpp>

#include "Types.h"

namespace Lumen
{
	class Entity;

	namespace Utils
	{
		class Serializer;
	}

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		Entity CreateEntity();
		bool GetEntity(u32 id, Entity& entity);
		void DestroyEntity(Entity entity);

	private:
		entt::registry mRegistry{};

		friend class Entity;
		friend class Utils::Serializer;
	};
}
