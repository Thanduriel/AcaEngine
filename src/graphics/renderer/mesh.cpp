#include "mesh.hpp"
#include "graphics/core/opengl.hpp"

namespace graphics {

	const VertexAttribute c_vertexAttributes[] =
	{
		VertexAttribute{PrimitiveFormat::FLOAT, 3, false, false},
		VertexAttribute{PrimitiveFormat::FLOAT, 3, false, false},
		VertexAttribute{PrimitiveFormat::FLOAT, 2, false, false},
	};

	Mesh::Mesh(const utils::MeshData& _meshData)
		: m_geomtryBuffer(GLPrimitiveType::TRIANGLES, c_vertexAttributes, 3, 4, sizeof(Vertex) * _meshData.positions.size())
	{
		std::vector<Vertex> vertices;
		vertices.reserve(_meshData.positions.size());

		for (size_t i = 0; i < _meshData.positions.size(); ++i)
		{
			Vertex v;
			v.position = _meshData.positions[i];
			v.normal = _meshData.positions[i];
			v.textureCoordinates = _meshData.textureCoordinates[i];
			vertices.push_back(v);
		}

		m_geomtryBuffer.setData(&vertices.front(), sizeof(Vertex) * vertices.size());

		const size_t n = _meshData.faces.size() * 3;
		std::vector<glm::uint32_t> indicies;
		indicies.reserve(n);

		for (size_t i = 0; i < _meshData.faces.size(); ++i)
		{
			indicies.emplace_back(_meshData.faces[i].indices[0].positionIdx-1);
			indicies.emplace_back(_meshData.faces[i].indices[1].positionIdx-1);
			indicies.emplace_back(_meshData.faces[i].indices[2].positionIdx-1);
		}
		m_geomtryBuffer.setIndexData(&indicies.front(), sizeof(uint32_t) * indicies.size());
	}

	void Mesh::draw()
	{
		m_geomtryBuffer.draw();
	}
}