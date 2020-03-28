#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace game {

	struct Position : public glm::vec3
	{
		using glm::vec3::vec3;
		Position(const glm::vec3& v) : glm::vec3{v}{} 
		Position(glm::vec3&& v) : glm::vec3{v}{} 
	};

	struct Velocity : public glm::vec3
	{
		using glm::vec3::vec3;
		Velocity(const glm::vec3& v) : glm::vec3{v}{}
		Velocity(glm::vec3& v) : glm::vec3{v}{}
	};

	struct Rotation : public glm::quat
	{
		using glm::quat::quat;
		Rotation(const glm::quat& q) : glm::quat{q}{}
		Rotation(glm::quat&& q) : glm::quat{q}{}
	};

	struct Transform : public glm::mat4
	{
		using glm::mat4::mat4;
		Transform(const glm::mat4& m) : glm::mat4{m}{}
		Transform(glm::mat4&& m) : glm::mat4{m}{}
	};

	struct Lifetime
	{
		Lifetime(float _time) : timeLeft(_time) {}

		float timeLeft;
	};
}
