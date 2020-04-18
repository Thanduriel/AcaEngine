#pragma once

#include <glm/glm.hpp>

namespace math {

	glm::mat2 rotation(float _angle)
	{
		const float sinA = sin(_angle);
		const float cosA = cos(_angle);
		return glm::mat2(cosA, -sinA,
			sinA, cosA);
	}
}