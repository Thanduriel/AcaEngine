#pragma once

#include <glm/glm.hpp>

namespace graphics {

	struct Camera
	{
		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 viewProjection;
	};
}