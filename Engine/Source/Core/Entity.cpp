#include "Entity.h"

namespace Lumen
{
	/**
	 * \brief Should be used only by Scene
	 * \param entity Entity handle
	 * \param scene Scene owner
	 */
	Entity::Entity(entt::entity entity, Scene* scene)
		: mScene{ scene }, mEntity{ entity }
	{

	}
}
