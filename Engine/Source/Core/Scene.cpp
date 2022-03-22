#include "Scene.h"

#include "Entity.h"

namespace Lumen
{
	Entity Scene::CreateEntity()
	{
		Entity* e = new Entity{ mRegistry.create(), this };
		mEntities.emplace_back(e);
		e->AddComponent<Components::Transform>();
		return *e;
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
		for (u32 i{ 0 }; i < mEntities.size(); i++)
		{
			if (mEntities[i]->mEntity == entity.mEntity)
			{
				mEntities.erase(mEntities.begin() + i);
				break;
			}
		}
		mRegistry.destroy(entity.mEntity);
	}
}
