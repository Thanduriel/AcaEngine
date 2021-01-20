#pragma once

#include "../core/shader.hpp"
#include "../camera.hpp"
#include "glm/glm.hpp"
#include "engine/game/components/lights.hpp"
#include "engine/game/operations/addPointLights.hpp"
#include <vector>
#include <array>

namespace graphics {

	class Mesh;
	class Texture2D;

	class MeshRenderer
	{
	public:
		float intensityFactor(float _d) { return 1.f / _d * _d; }
		static constexpr int MAX_LIGHTS = 8;
		MeshRenderer();
		struct PointLights {
			using elm_t = game::operations::GrepPointLights::PointLight;
			template<typename ... LIGHTS>
			PointLights(LIGHTS ... _lights)
			:
				color{{_lights.color...}},
				position{{_lights.position...}},
				intensity{{_lights.intensity...}},
				size(sizeof...(_lights)) {}
			PointLights(const glm::vec3& _objPosition, const std::vector<elm_t>& _lights)
			{
				std::array<float, MAX_LIGHTS> scores;
				memset(scores.data(), 0, sizeof(float)*MAX_LIGHTS);
				for(int i = 0; i < _lights.size(); ++i)
				{
					glm::vec3 diff = _objPosition - _lights[i].position;
					float score = glm::dot(diff,diff);
					if (i == 0) {
						color[i] = _lights[i].color;
						position[i] = _lights[i].position;
						intensity[i] = _lights[i].intensity;
						scores[i] = score;
					}
					else if (i < MAX_LIGHTS || score > scores.back())
					{
						int j;
						for( j = std::min(i,MAX_LIGHTS); j > 0; --j)
						{
							if (scores[j-1] < score) {
								if (j == MAX_LIGHTS)
								{
									continue;
								}
								color[j] = color[j-1];
								position[j] = position[j-1];
								intensity[j] = intensity[j-1];
								scores[j] = scores[j-1];
							}
							else
							{
								break;
							}
						}
						if (j < MAX_LIGHTS) {
							color[j] = _lights[i].color;
							position[j] = _lights[i].position;
							intensity[j] = _lights[i].intensity;
							scores[j] = score;
						}
					}
				}
				size = std::min((int)_lights.size(), MAX_LIGHTS);
			}
			std::array<glm::vec3, MAX_LIGHTS> color;
			std::array<glm::vec3, MAX_LIGHTS> position;
			std::array<float, MAX_LIGHTS> intensity;
			int size;
		};

		void draw(const Mesh& _mesh, const Texture2D& _texture, const glm::mat4& _affineTransform, const glm::mat4& _projection, const PointLights& _lights);
		void addLight(const glm::vec3& _color, const glm::vec3& _position, float intensity);
		void present(const glm::vec3& _camera_position);
		void clear();
	private:
		std::vector<const Mesh*> m_meshes;
		std::vector<const Texture2D*> m_textures;
		std::vector<glm::mat4> m_transforms;
		std::vector<glm::mat4> m_normalTransforms;
		std::vector<glm::mat4> m_worldTransforms;
		std::vector<PointLights> m_lights;
		Program m_program;
	};
}
