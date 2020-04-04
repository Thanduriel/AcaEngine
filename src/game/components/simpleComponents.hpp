#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace game { namespace components{

	struct Position 
	{
		Position(const glm::vec3& v) : value{v}{} 
		Position(glm::vec3&& v) : value{v}{} 

		glm::vec3 value;
	};

	struct Velocity 
	{
		Velocity(const glm::vec3& v) : value{v}{}
		Velocity(glm::vec3& v) : value{v}{}

		glm::vec3 value;
	};

	struct Rotation 
	{
		Rotation(const glm::quat& q) : value{q}{}
		Rotation(glm::quat&& q) : value{q}{}

		glm::quat value;
	};

	struct Transform 
	{
		Transform(const glm::mat4& m) : value{m}{}
		Transform(glm::mat4&& m) : value{m}{}

		glm::mat4 value;
	};

	struct Lifetime
	{
		Lifetime(float _time) : timeLeft(_time) {}

		float timeLeft;
	};
}}
