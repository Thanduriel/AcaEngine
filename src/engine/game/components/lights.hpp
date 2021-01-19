#pragma once

#include <glm/glm.hpp>

namespace game { namespace components {
	struct PointLight
	{
		PointLight(const glm::vec3& color, float intensity)
			: color{color}, intensity{intensity} {}
		PointLight(glm::vec3&& color, float intensity)
			: color{color}, intensity{intensity} {}
		glm::vec3 color;
		float intensity;
	};
} } // end namespace game
