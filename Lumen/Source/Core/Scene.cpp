#include "Scene.h"

#include "Entity.h"

namespace Lumen
{
	Entity Scene::CreateEntity()
	{
		const Entity e{ mRegistry.create(), this };
		return e;
	}

	bool Scene::GetEntity(u32 id, Entity& entity)
	{
		bool found{ false };
		mRegistry.each([&](auto enttId)
		{
			if (static_cast<u32>(enttId) == id)
			{
				found = true;
				return;
			}
		});

		entity.mEntity = static_cast<entt::entity>(found ? id : -1);
		entity.mScene = this;

		return found;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		mRegistry.destroy(entity.mEntity);
	}
}
