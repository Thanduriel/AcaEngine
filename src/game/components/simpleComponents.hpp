#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace game {

	struct Position : public glm::vec3
	{
		using glm::vec3::vec3;
	};

	struct Velocity : public glm::vec3
	{
		using glm::vec3::vec3;
	};

	struct Rotation : public glm::quat
	{
		using glm::quat::quat;
	};

	struct Transform : public glm::mat4
	{
		using glm::mat4::mat4;
	};

	struct Lifetime
	{
		Lifetime(float _time) : timeLeft(_time) {}

		float timeLeft;
	};
}