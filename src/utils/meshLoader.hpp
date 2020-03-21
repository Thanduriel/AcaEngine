#pragma once

#include "utils/resourcemanager.hpp"

#include "glm/vec3.hpp"
#include "glm/vec2.hpp"

#include <vector>
#include <array>
#include <optional>

namespace utils {

	struct MeshData 
	{
		struct FaceData 
		{
			struct VertexIndices 
			{
				int positionIdx;
				std::optional<int> textureCoordinateIdx;
				std::optional<int> normalIdx;
			};
			std::array<VertexIndices, 3> indices;
		};

		using Handle = const MeshData*;
		
		static Handle load( const char* _fileName ) ;
		static void unload( Handle _meshData );

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> textureCoordinates;
		std::vector<glm::vec3> normals;
		std::vector<FaceData>  faces;
	};

	using MeshLoader = utils::ResourceManager<MeshData>;

} // end utils
