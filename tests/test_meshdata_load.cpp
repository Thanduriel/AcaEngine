#include "utils/meshLoader.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <optional>

#ifndef RESOURCE_FOLDER
#define RESOURCE_FOLDER "./"
#endif

namespace std {
	string to_string(const optional<int>&  val) {
		return val ? std::to_string(val.value()) : std::string("nullopt");
	}
} // end namespace std

template<typename T>
std::string faceMismatchError(
		int fId, int vId, 
		const std::string& attr,
		const T& get,
		const T& expected)
{
		return std::string("expected face ")
			+ std::to_string(fId) + " vertex " + std::to_string(vId)
			+ " " + attr + " to be " + std::to_string(expected)
			+ " but got " +  std::to_string(get);
}

int main() {
	try {
		const utils::MeshData* data = utils::MeshLoader::get( 
				RESOURCE_FOLDER "/cube.obj" );
		std::array<utils::MeshData::FaceData, 16> faces;
			faces[0].indices[0].positionIdx = 0;
			faces[0].indices[0].textureCoordinateIdx = 0; 
			faces[0].indices[0].normalIdx = 0;
			faces[0].indices[1].positionIdx = 3;
			faces[0].indices[1].textureCoordinateIdx = 3;
			faces[0].indices[1].normalIdx = 0;
			faces[0].indices[2].positionIdx = 1;
			faces[0].indices[2].textureCoordinateIdx = 1;
			faces[0].indices[2].normalIdx = 0;

			faces[1].indices[0].positionIdx = 0;
			faces[1].indices[0].textureCoordinateIdx = 0; 
			faces[1].indices[0].normalIdx = 1;
			faces[1].indices[1].positionIdx = 2;
			faces[1].indices[1].textureCoordinateIdx = 2;
			faces[1].indices[1].normalIdx = 1;
			faces[1].indices[2].positionIdx = 3;
			faces[1].indices[2].textureCoordinateIdx = 3;
			faces[1].indices[2].normalIdx = 1;

			faces[2].indices[0].positionIdx = 0;
			faces[2].indices[0].textureCoordinateIdx = 0; 
			faces[2].indices[0].normalIdx = 2;
			faces[2].indices[1].positionIdx = 1;
			faces[2].indices[1].textureCoordinateIdx = 1;
			faces[2].indices[1].normalIdx = 2;
			faces[2].indices[2].positionIdx = 2;
			faces[2].indices[2].textureCoordinateIdx = 2;
			faces[2].indices[2].normalIdx = 2;

			faces[3].indices[0].positionIdx = 3;
			faces[3].indices[0].textureCoordinateIdx = 3; 
			faces[3].indices[0].normalIdx = 3;
			faces[3].indices[1].positionIdx = 1;
			faces[3].indices[1].textureCoordinateIdx = 1;
			faces[3].indices[1].normalIdx = 3;
			faces[3].indices[2].positionIdx = 2;
			faces[3].indices[2].textureCoordinateIdx = 2;
			faces[3].indices[2].normalIdx = 3;

			for (int i = 4; i < 8; ++i) {
				faces[i] = faces[i - 4];
				for(auto& idx : faces[i].indices) {
					idx.textureCoordinateIdx = std::nullopt;
				}
			}

			for (int i = 8; i < 12; ++i) {
				faces[i] = faces[i - 8];
				for(auto& idx : faces[i].indices) {
					idx.normalIdx = std::nullopt;
				}
			}

			for (int i = 12; i < 16; ++i) {
				faces[i] = faces[i - 12];
				for(auto& idx : faces[i].indices) {
					idx.textureCoordinateIdx = std::nullopt;
					idx.normalIdx = std::nullopt;
				}
			}

		if ( data->faces.size() != faces.size() ) {
			throw std::string("expected ") 
				+ std::to_string(faces.size()) + " faces, got: '" 
				+ std::to_string(data->faces.size()) +  "'";
		}

		for ( auto i = 0; i < faces.size(); ++i) {
			const utils::MeshData::FaceData& expected = faces[i];
			const utils::MeshData::FaceData& readed = data->faces[i];
			for ( int x = 0; x < 3; ++x ) 
			{
				if ( readed.indices[x].positionIdx 
						!= expected.indices[x].positionIdx ) 
				{
					throw faceMismatchError(i, x, "positionIdx", 
							readed.indices[x].positionIdx,
							expected.indices[x].positionIdx);
				}
				if ( readed.indices[x].normalIdx 
						!= expected.indices[x].normalIdx ) 
				{
					throw faceMismatchError(i, x, "normalIdx", 
							readed.indices[x].normalIdx,
							expected.indices[x].normalIdx);
				}
				if ( readed.indices[x].textureCoordinateIdx 
						!= expected.indices[x].textureCoordinateIdx ) 
				{
					throw faceMismatchError(i, x, "textureCoordinateIdx", 
							readed.indices[x].textureCoordinateIdx,
							expected.indices[x].textureCoordinateIdx);
				}
			}
		}
			
	} catch (const std::string& msg) {
		std::cerr << msg << std::endl;
		return 1;
	}
	return 0;
}
