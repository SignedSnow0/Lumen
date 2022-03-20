#pragma once
#include <vector>

#include <entt/entt.hpp>

#include "Types.h"

namespace Lumen
{
	namespace Graphics
	{
		class SceneRenderer;
	}

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
		std::vector<Entity*>& Entities() { return mEntities; }

	private:
		entt::registry mRegistry{};
		std::vector<Entity*> mEntities;

		friend class Entity;
		friend class Utils::Serializer;
		friend class Graphics::SceneRenderer;
	};
}
