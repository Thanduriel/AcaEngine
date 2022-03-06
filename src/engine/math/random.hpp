#pragma once

#include "xoshiro.hpp"
#include <glm/gtc/quaternion.hpp>
#include <random>
#include <numbers>

namespace math {
namespace random {

	using DefaultRandomEngine = xoshiro128ss;
	extern thread_local DefaultRandomEngine g_random;

	// Generates uniformly distributed 3D rotations.
	template <typename T = float,
		class Engine = DefaultRandomEngine>
	glm::qua<T> rotation(Engine& _engine = g_random)
	{
		std::uniform_real_distribution<T> uniform;
		
		const T s = uniform(_engine);
		const T o1 = sqrt(1.f - s);
		const T o2 = sqrt(s);
		const T t1 = 2.f * std::numbers::pi_v<T> * uniform(_engine);
		const T t2 = 2.f * std::numbers::pi_v<T> * uniform(_engine);
		const T w = cos(t2) * o2;
		const T x = sin(t1) * o1;
		const T y = cos(t1) * o1;
		const T z = sin(t2) * o2;
		return glm::qua<T>(x, y, z, w);
	}

	// Generates uniformly distributed random points on the unit sphere.
	template <typename T = float, 
		class Engine = DefaultRandomEngine>
	glm::vec<3,T> direction(Engine& _engine = g_random)
	{
		std::uniform_real_distribution<T> uniform;

		const T phi = 2 * std::numbers::pi_v<T> * uniform(_engine);
		const T cosTheta = 2.0f * uniform(_engine) - 1.0f;
		const T sinTheta = sqrt((1.0f - cosTheta) * (1.0f + cosTheta));
		return glm::vec<3,T>(sinTheta * sin(phi), sinTheta * cos(phi), cosTheta);
	}
}}