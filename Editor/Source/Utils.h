#pragma once
#include <glm/glm.hpp>
#include <Lumen.h>

b8 DrawVec3(const std::string& label, glm::vec3& vec, const glm::vec3& defaultValue = glm::vec3{ 0.0f });
b8 DrawVec2(const std::string& label, glm::vec2& vec, const glm::vec2& defaultValue = glm::vec2{ 0.0f });
