#pragma once

#include "../core/shader.hpp"
#include "../camera.hpp"
#include "glm/glm.hpp"
#include "engine/game/components/lights.h"
#include <vector>

namespace graphics {

	class Mesh;
	class Texture2D;

	class MeshRenderer
	{
	public:
		MeshRenderer();

		void draw(const Mesh& _mesh, const Texture2D& _texture, const glm::mat4& _affineTransform, const glm::mat4& _projection, std::array<game::components::PointLight, MAX_LIGHTS>&& _lights)
		void addLight(const glm::vec3& _color, const glm::vec3& _position, float intensity);
		void present();
		void clear();
	private:
		static constexpr int MAX_LIGHTS = 8;
		std::vector<const Mesh*> m_meshes;
		std::vector<const Texture2D*> m_textures;
		std::vector<glm::mat4> m_transforms;
		std::vector<glm::mat4> m_normalTransforms;
		std::vector<glm::mat4> m_worldTransforms;
		std::vector<std::array<game::components::PointLight,MAX_LIGHTS>> m_lights;
		Program m_program;
	};
}
