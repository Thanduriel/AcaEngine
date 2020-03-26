#include "meshrenderer.hpp"
#include "mesh.hpp"

namespace graphics {

	MeshRenderer::MeshRenderer()
	{
		m_program.attach(ShaderManager::get("../resources/shader/model.vert", ShaderType::VERTEX));
		m_program.attach(ShaderManager::get("../resources/shader/model.frag", ShaderType::FRAGMENT));
		m_program.link();
	}

	void MeshRenderer::draw(const Mesh& _mesh, const glm::mat4& _transform)
	{
		m_meshes.push_back(&_mesh);
		m_transforms.push_back(_transform);
	}

	void MeshRenderer::present()
	{
		m_program.use();
		for(size_t i = 0; i < m_meshes.size(); ++i)
		{
			m_program.setUniform(0, m_transforms[i]);
			m_program.setUniform(1, m_transforms[i]);
			m_meshes[i]->m_geomtryBuffer.draw();
		}
	}

	void MeshRenderer::clear()
	{
		m_meshes.clear();
		m_transforms.clear();
	}
}
