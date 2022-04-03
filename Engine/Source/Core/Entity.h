#pragma once
#include "Scene.h"
#include "Components.h"

#include <entt/entt.hpp>

namespace Lumen
{
	class Entity
	{
	public:
		/**
		 * \brief Creates an invalid entity, use Scene::CreateEntity() or Scene::GetEntity() instead;
		 */
		Entity() = default;
		explicit Entity(entt::entity entity, Scene* scene);
		~Entity() = default;

		/**
		 * \brief Returns true if the entity has all the components specified in the template parameter.
		 * \tparam TComp Components to check for.
		 */
		template<typename ... TComp>
		[[nodiscard]] bool HasComponents() const
		{
			static_assert(std::is_base_of_v<Components::Component, TComp ...>, "TComp must inherit from Component");
			return mScene->mRegistry.all_of<TComp ...>(mEntity);
		}

		/**
		 * \brief Adds a component to the entity or returns the old one if it already has one of the same type.
		 * \tparam TComp Component to add.
		 * \tparam TArgs Constructor argument types.
		 * \param args Constructor arguments.
		 * \return A reference to the new or old component
		 */
		template<typename TComp, typename ... TArgs>
		TComp& AddComponent(TArgs ... args)
		{
			static_assert(std::is_base_of_v<Components::Component, TComp>, "TComp must inherit from Component");
			TComp* component = mScene->mRegistry.try_get<TComp>(mEntity);
			return component ? *component : mScene->mRegistry.emplace<TComp>(mEntity, args ...);
		}

		/**
		 * \brief Returns a pointer to the selected components of the entity.
		 * \tparam TComp Components to get.
		 */
		template<typename ... TComp>
		decltype(auto) GetComponents() const
		{
			static_assert(std::is_base_of_v<Components::Component, TComp ...>, "TComp must inherit from Component");
			return mScene->mRegistry.try_get<TComp ...>(mEntity);
		}

		/**
		 * \brief Removes the specified components from the entity.
		 * \tparam TComp The components to remove.
		 */
		template<typename ... TComp>
		void DeleteComponent() const
		{
			static_assert(std::is_base_of_v<Components::Component, TComp>, "TComp must inherit from Component");
			mScene->mRegistry.remove<TComp ...>(mEntity);
		}

		/**
		 * \brief Returns the id of the entity.
		 */
		[[nodiscard]] u32 GetId() const { return static_cast<u32>(mEntity); }

	private:
		Scene*			mScene{ nullptr };
		entt::entity	mEntity{ static_cast<entt::entity>(-1) };

		friend class Utils::Serializer;
		friend class Scene;
	};
}
