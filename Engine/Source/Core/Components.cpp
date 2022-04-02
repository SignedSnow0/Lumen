#include "Components.h"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Lumen::Components
{
	glm::mat4 Transform::GetTransform() const
	{
		return glm::translate(glm::mat4{ 1.0f }, Translation) * glm::toMat4(glm::quat{ glm::radians(Rotation) }) * glm::scale(glm::mat4{ 1.0f }, Scale);
	}
}
