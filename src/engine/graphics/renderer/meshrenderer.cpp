#include "meshrenderer.hpp"
#include "mesh.hpp"
#include "../core/opengl.hpp"
#include "../core/texture.hpp"
#include "../resources.hpp"
#include <glm/gtx/compatibility.hpp>

namespace graphics {

	MeshRenderer::MeshRenderer()
	{
		m_program.attach(ShaderManager::get("../resources/shader/model.vert", ShaderType::VERTEX));
		m_program.attach(ShaderManager::get("../resources/shader/model.frag", ShaderType::FRAGMENT));
		m_program.link();
	}

	void MeshRenderer::draw(const Mesh& _mesh, const Texture2D& _texture, const glm::mat4& _affineTransform, const glm::mat4& _projection)
	{
		m_meshes.push_back(&_mesh);
		m_textures.push_back(&_texture);
		m_transforms.push_back(_projection * _affineTransform);
		m_normalTransforms.push_back(glm::transpose(glm::inverse(_affineTransform)));
	}

	void MeshRenderer::present()
	{
		m_program.use();
		glEnable(GL_DEPTH_TEST);

		for(size_t i = 0; i < m_meshes.size(); ++i)
		{
			m_program.setUniform(0, m_transforms[i]);
			m_program.setUniform(1, m_normalTransforms[i]);

			m_textures[i]->bind(0);
			m_meshes[i]->m_geomtryBuffer.draw();
		}
	}

	void MeshRenderer::clear()
	{
		m_meshes.clear();
		m_textures.clear();
		m_transforms.clear();
	}
}
