#pragma once

#include "graphics/core/shader.hpp"
#include "glm/glm.hpp"
#include <vector>

namespace graphics {

	class Mesh;

	class MeshRenderer
	{
	public:
		MeshRenderer();

		void draw(const Mesh& _mesh, const glm::mat4& _transform);

		void present();
		void clear();
	private:
		std::vector<const Mesh*> m_meshes;
		std::vector<glm::mat4> m_transforms;
		Program m_program;
	};
}