#pragma once

#include <glm/gtc/quaternion.hpp>
#include <random>
#include <numbers>

namespace math {
namespace random {

	// todo: replace with a fast random engine
	using DefaultRandomEngine = std::default_random_engine;
	extern thread_local DefaultRandomEngine g_random;

	template <class Engine = DefaultRandomEngine>
	glm::quat rotation(Engine& _engine = g_random)
	{
		std::uniform_real_distribution<float> uniform;
		
		float s = uniform(_engine);
		float o1 = sqrt(1.f - s);
		float o2 = sqrt(s);
		float t1 = 2.f * std::numbers::pi_v<float> * uniform(_engine);
		float t2 = 2.f * std::numbers::pi_v<float> * uniform(_engine);
		float w = cos(t2) * o2;
		float x = sin(t1) * o1;
		float y = cos(t1) * o1;
		float z = sin(t2) * o2;
		return glm::quat(x, y, z, w);
	}

	template <class Engine = DefaultRandomEngine>
	glm::vec3 direction(Engine& _engine = g_random)
	{
		std::uniform_real_distribution<float> uniform;

		float phi = 2 * std::numbers::pi_v<float> * uniform(_engine);
		float cosTheta = 2.0f * uniform(_engine) - 1.0f;
		float sinTheta = sqrt((1.0f - cosTheta) * (1.0f + cosTheta));
		return glm::vec3(sinTheta * sin(phi), sinTheta * cos(phi), cosTheta);
	}
}}