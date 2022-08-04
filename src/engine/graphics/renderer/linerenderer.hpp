#pragma once

#include <engine/graphics/core/geometrybuffer.hpp>
#include <engine/graphics/core/shader.hpp>
#include <engine/graphics/camera.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace graphics {
	class LineRenderer
	{
	public:
		LineRenderer();

		// Add a line to the set.
		void draw(const glm::vec3& _start, const glm::vec3& _end, const glm::vec4& _color, float _thickness);

		void present(const Camera& _camera);
		void clear();
	private:
		struct Vertex
		{
			glm::vec3 start;
			glm::vec3 end;
			glm::vec4 color;
			float thickness;
		};

		std::vector<Vertex> m_vertices;
		GeometryBuffer m_buffer;
		Program m_program;
	};
}