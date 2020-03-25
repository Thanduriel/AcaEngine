#pragma once

#include "utils/meshLoader.hpp"
#include "graphics/core/geometrybuffer.hpp"
#include "glm/glm.hpp"

namespace graphics {

	class Mesh
	{
	public:
		Mesh(const utils::MeshData& _meshData);

		void draw();
	private:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 textureCoordinates;
		};
		
		GeometryBuffer m_geomtryBuffer;
	};
}