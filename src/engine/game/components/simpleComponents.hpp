#pragma once

#include "../core/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace game { namespace components {

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

	struct AngularVelocity
	{
		AngularVelocity() : axis(1.f, 0.f, 0.f), speed(0.f) {}
		// normalized axis and angular speed in radians
		AngularVelocity(const glm::vec3& _axis, float _speed)
			: axis(_axis), speed(_speed)
		{
			assert(std::abs(1.f - glm::length(_axis)) < 0.0001f);
		}

		glm::vec3 axis;
		float speed;
	};

	struct Scale
	{
		Scale(const glm::vec3& v) : value(v) {}
		glm::vec3 value;
	};

	struct Transform 
	{
		Transform() : value(glm::identity<glm::mat4>()) {}
		Transform(const glm::mat4& m) : value{m}{}
		Transform(glm::mat4&& m) : value{m}{}

		glm::mat4 value;
	};

	struct TransformNeedsUpdate {};

	struct Lifetime
	{
		Lifetime(float _time) : timeLeft(_time) {}

		float timeLeft;
	};

	// temporarily mark a component to find it again
	struct Mark {};
}}
