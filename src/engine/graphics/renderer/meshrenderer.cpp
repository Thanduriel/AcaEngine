#include "meshrenderer.hpp"
#include "mesh.hpp"
#include "engine/game/components/lights.hpp"
#include "../core/opengl.hpp"
#include "../core/texture.hpp"
#include <glm/gtx/compatibility.hpp>

namespace graphics {

	MeshRenderer::MeshRenderer()
	{
		m_program.attach(ShaderManager::get("../resources/shader/light.vert", ShaderType::VERTEX));
		m_program.attach(ShaderManager::get("../resources/shader/light.frag", ShaderType::FRAGMENT));
		m_program.link();
	}

	void MeshRenderer::draw(const Mesh& _mesh, const Texture2D& _texture, const glm::mat4& _affineTransform, const glm::mat4& _projection, const PointLights& _lights)
	{
		m_meshes.push_back(&_mesh);
		m_textures.push_back(&_texture);
		m_transforms.push_back(_projection * _affineTransform);
		m_worldTransforms.push_back(_affineTransform);
		m_normalTransforms.push_back(glm::transpose(glm::inverse(_affineTransform)));
		m_lights.push_back(_lights);
	}

	void MeshRenderer::present()
	{
		m_program.use();
		glEnable(GL_DEPTH_TEST);

		for(size_t i = 0; i < m_meshes.size(); ++i)
		{
			const auto& light = m_lights[i];
			const int nLights = m_lights[i].size;
			m_program.setUniform(0, m_transforms[i]);
			m_program.setUniform(1, m_normalTransforms[i]);
			m_program.setUniform(2, m_worldTransforms[i]);
			m_program.setUniform(3 , light.color[0], nLights);
			m_program.setUniform(3 + MAX_LIGHTS, light.position[0], nLights);
			m_program.setUniform(3 + 2 * MAX_LIGHTS, light.intensity[0], nLights);
			m_program.setUniform(3 + 3 * MAX_LIGHTS, nLights);


			m_textures[i]->bind(0);
			m_meshes[i]->m_geomtryBuffer.draw();
		}
	}

	void MeshRenderer::clear()
	{
		m_meshes.clear();
		m_textures.clear();
		m_transforms.clear();
		m_lights.clear();
	}
}
