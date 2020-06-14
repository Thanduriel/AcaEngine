#include "mesh.hpp"
#include "../core/opengl.hpp"

namespace graphics {

	const static VertexAttribute c_vertexAttributes[] =
	{
		VertexAttribute{PrimitiveFormat::FLOAT, 3, false, false},
		VertexAttribute{PrimitiveFormat::FLOAT, 3, false, false},
		VertexAttribute{PrimitiveFormat::FLOAT, 2, false, false},
	};

	Mesh::Mesh(const utils::MeshData& _meshData)
		: m_geomtryBuffer(GLPrimitiveType::TRIANGLES, c_vertexAttributes, 3, 0, sizeof(Vertex) * _meshData.faces.size()*3)
	{
		std::vector<Vertex> vertices;
		vertices.reserve(_meshData.faces.size() * 3);

	/*	for (size_t i = 0; i < _meshData.positions.size(); ++i)
		{
			Vertex v;
			v.position = _meshData.positions[i];
			v.normal = _meshData.normals[i];
			v.texCoords = _meshData.textureCoordinates[i];
			vertices.push_back(v);
		}*/

		const size_t n = _meshData.faces.size() * 3;
	//	std::vector<glm::uint32_t> indicies;
	//	indicies.reserve(n);

		for (size_t i = 0; i < _meshData.faces.size(); ++i)
		{
			const auto& indices = _meshData.faces[i].indices;
		/*	indicies.emplace_back(indices[0].positionIdx);
			indicies.emplace_back(indices[1].positionIdx);
			indicies.emplace_back(indices[2].positionIdx);*/

			for (int j = 0; j < 3; ++j)
			{
				Vertex v;
				v.position = _meshData.positions[indices[j].positionIdx];
				v.normal = _meshData.normals[*indices[j].normalIdx];
				v.texCoords = _meshData.textureCoordinates[*indices[j].textureCoordinateIdx];
				v.texCoords.y = 1.f - v.texCoords.y;
				vertices.push_back(v);
			}
		}

		m_geomtryBuffer.setData(&vertices.front(), sizeof(Vertex) * vertices.size());
	//	m_geomtryBuffer.setIndexData(&indicies.front(), sizeof(uint32_t) * indicies.size());
	}
}