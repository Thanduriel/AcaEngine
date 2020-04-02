#pragma once

#include "graphics/core/shader.hpp"
#include "graphics/camera.hpp"
#include "glm/glm.hpp"
#include <vector>

namespace graphics {

	class Mesh;
	class Texture2D;

	class MeshRenderer
	{
	public:
		MeshRenderer();

		void draw(const Mesh& _mesh, const Texture2D& _texture, const glm::mat4& _affineTransform, const glm::mat4& _projection);

		void present();
		void clear();
	private:
		std::vector<const Mesh*> m_meshes;
		std::vector<const Texture2D*> m_textures;
		std::vector<glm::mat4> m_transforms;
		std::vector<glm::mat4> m_normalTransforms;
		Program m_program;
	};
}