#pragma once

#include "glm/mat4x4.hpp"

namespace graphics {
	class Mesh;
}

namespace game {

	struct Model
	{
		Model(const graphics::Mesh& _mesh, const glm::mat4& _transform)
			: mesh(&_mesh), transform(_transform)
		{}

		const graphics::Mesh* mesh;
		glm::mat4 transform;
	};

}