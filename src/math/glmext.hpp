#pragma once

#include <glm/glm.hpp>

namespace Math {

	glm::mat2 Rotation(float _angle)
	{
		const float sinA = sin(_angle);
		const float cosA = cos(_angle);
		return glm::mat2(cosA, -sinA,
			sinA, cosA);
	}
}