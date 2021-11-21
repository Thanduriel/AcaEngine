#pragma once

#include <glm/glm.hpp>

namespace math {

	inline glm::mat2 rotation(float _angle)
	{
		const float sinA = sin(_angle);
		const float cosA = cos(_angle);
		return glm::mat2(cosA, -sinA,
			sinA, cosA);
	}

	// todo: check whether the internally used mat3->mat4 in quat::tomat4 is better
	inline glm::mat4 extendTransform(const glm::mat3& _transform)
	{
		glm::mat4 mat{};
		mat[0][0] = _transform[0][0];
		mat[0][1] = _transform[0][1];
		mat[1][0] = _transform[1][0];
		mat[1][1] = _transform[1][1];
		mat[3][0] = _transform[2][0];
		mat[3][1] = _transform[2][1];
		mat[3][3] = _transform[2][2];

		return mat;
	}
}