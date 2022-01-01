#include "Entity.h"

namespace Lumen
{
	Entity::Entity(entt::entity entity, Scene* scene)
		: mScene{ scene }, mEntity{ entity }
	{

	}
}
