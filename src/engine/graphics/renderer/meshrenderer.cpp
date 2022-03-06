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

	void MeshRenderer::draw(const Mesh& _mesh, const Texture2D& _texture, const glm::mat4& _transform)
	{
		m_meshes.push_back(&_mesh);
		m_textures.push_back(&_texture);
		m_transforms.push_back(_transform);
	}

	void MeshRenderer::present(const graphics::Camera& _camera)
	{
		Device::setZFunc(ComparisonFunc::LESS);
		Device::setCullMode(CullMode::BACK);
		Device::enableBlending(false);

		m_program.use();
		m_program.setUniform(3, glm::normalize(glm::vec3(1.f, 1.f, 1.f)));

		for(size_t i = 0; i < m_meshes.size(); ++i)
		{
			const glm::mat4 mv = _camera.getView() * m_transforms[i];
			const glm::mat4 mvp = _camera.getViewProjection() * m_transforms[i];
			const glm::mat4 normalTransform = glm::transpose(glm::inverse(mv));
			m_program.setUniform(0, mvp);
			m_program.setUniform(1, normalTransform);
			m_program.setUniform(2, mv);

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
