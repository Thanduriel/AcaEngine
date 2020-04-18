#pragma once

#include "../core/component.hpp"
#include <glm/mat4x4.hpp>

namespace graphics {
	class Mesh;
	class Texture2D;
}

namespace game { namespace components {

	struct Model : public MultiComponent
	{
		Model(const graphics::Mesh& _mesh, const graphics::Texture2D& _texture, const glm::mat4& _transform)
			: mesh(&_mesh), texture(&_texture), transform(_transform)
		{}

		const graphics::Mesh* mesh;
		const graphics::Texture2D* texture;
		glm::mat4 transform;
	};

}}