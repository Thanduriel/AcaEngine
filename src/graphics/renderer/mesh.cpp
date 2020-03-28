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

		const size_t n = _meshData.faces.size() * 3;
		std::vector<glm::uint32_t> indicies;
		indicies.reserve(n);

		for (size_t i = 0; i < _meshData.faces.size(); ++i)
		{
			const auto& indices = _meshData.faces[i].indices;
			indicies.emplace_back(indices[0].positionIdx);
			indicies.emplace_back(indices[1].positionIdx);
			indicies.emplace_back(indices[2].positionIdx);

			// overwrite normals if available
			if (indices[0].normalIdx) vertices[indices[0].positionIdx].normal = _meshData.normals[*indices[0].normalIdx];
			if (indices[1].normalIdx) vertices[indices[1].positionIdx].normal = _meshData.normals[*indices[1].normalIdx];
			if (indices[2].normalIdx) vertices[indices[2].positionIdx].normal = _meshData.normals[*indices[2].normalIdx];
		}

		m_geomtryBuffer.setData(&vertices.front(), sizeof(Vertex) * vertices.size());
		m_geomtryBuffer.setIndexData(&indicies.front(), sizeof(uint32_t) * indicies.size());
	}
}