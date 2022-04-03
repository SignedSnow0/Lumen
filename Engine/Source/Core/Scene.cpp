#include "Scene.h"

#include "Components.h"
#include "Entity.h"
#include "Utils/Logger.h"

namespace Lumen
{
	/**
	 * \brief Creates a new entity for this scene.
	 */
	Entity Scene::CreateEntity()
	{
		auto e = new Entity{ mRegistry.create(), this };
		mEntities.emplace_back(e);
		e->AddComponent<Components::Transform>();

		LOG_TRACE("Created entity with id {}", e->GetId());

		return *e;
	}

	/**
	 * \brief If the entity is exists in the scene the function will get it in the entity.
	 * \param id The entity id to check for.
	 * \param entity The entity to get.
	 * \return True if the entity exists in the scene.
	 */
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

	/**
	 * \brief Removes the entity from the scene.
	 * \param entity The entity to remove.
	 */
	void Scene::DestroyEntity(const Entity entity)
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

		LOG_TRACE("Destroyed entity with id {}", entity.mEntity);
	}

	/**
	 * \brief Returns all the entities in the scene
	 */
	std::vector<Entity*>& Scene::GetEntities() { return mEntities; }
}
