#pragma once
#include <entt/entt.hpp>

#include "Scene.h"
#include "Components.h"

namespace Lumen
{
	class Entity
	{
	public:
		Entity() = default;
		explicit Entity(entt::entity entity, Scene* scene);
		~Entity() = default;

		template<typename ... TComp>
		[[nodiscard]] bool HasComponents() const
		{
			static_assert(std::is_base_of_v<Components::Component, TComp ...>, "TComp must inherit from Component");
			return mScene->mRegistry.all_of<TComp ...>(mEntity);
		}

		template<typename TComp, typename ... TArgs>
		TComp& AddComponent(TArgs ... args)
		{
			static_assert(std::is_base_of_v<Components::Component, TComp>, "TComp must inherit from Component");
			TComp* component = mScene->mRegistry.try_get<TComp>(mEntity);
			return component ? *component : mScene->mRegistry.emplace<TComp>(mEntity, args ...);
		}

		template<typename ... TComp>
		[[nodiscard]] decltype(auto) GetComponents() const
		{
			static_assert(std::is_base_of_v<Components::Component, TComp ...>, "TComp must inherit from Component");
			return mScene->mRegistry.try_get<TComp ...>(mEntity);
		}

		template<typename ... TComp>
		void DeleteComponent(TComp ...)
		{
			static_assert(std::is_base_of_v<Components::Component, TComp>, "TComp must inherit from Component");
			mScene->mRegistry.remove<TComp ...>(mEntity);
		}

		[[nodiscard]] u32 Id() const { return static_cast<u32>(mEntity); }

	private:
		Scene*			mScene{ nullptr };
		entt::entity	mEntity{ static_cast<entt::entity>(-1) };

		friend class Utils::Serializer;
		friend class Scene;
	};
}
