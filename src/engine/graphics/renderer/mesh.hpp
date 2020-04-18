#pragma once

#include "../../utils/meshLoader.hpp"
#include "../core/geometrybuffer.hpp"
#include <glm/glm.hpp>

namespace graphics {

	class Mesh
	{
	public:
		Mesh(const utils::MeshData& _meshData);

	private:
		friend class MeshRenderer;

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 texCoords;
		};
		
		GeometryBuffer m_geomtryBuffer;
	};
}