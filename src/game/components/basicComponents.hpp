#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace game {

	struct Position : public glm::vec3
	{
		using glm::vec3::vec3;
	};

	struct Rotation : public glm::quat
	{
		using glm::quat::quat;
	};
}