#include "utils/meshLoader.hpp"

#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include <exception>
#include <tuple>
#include <cctype>

#include <iostream>

class parsing_error : public std::exception {
public:
	parsing_error(const std::string& msg) : m_msg{msg} {} 
	const char* what() const noexcept override { return m_msg.c_str(); }
private:
	std::string m_msg;
};

void errorTypeNotSupported(std::string_view _fileName) {
	spdlog::error("[utils] Could not load '", _fileName, "' "
	"type not supported. Only support '.obj'");
}


glm::vec3 parseVec3(const std::string& line, std::string::size_type begin) {
	std::string::size_type pos = begin, diff;
	glm::vec3 res;
	for(int i = 0; i < 3; ++i) 
	{
		float val = std::stof(line.substr(pos), &diff);
		if ( std::fabs(val) == 0 ) val = 0;
		res[i] = val;
		pos += diff;
	}
	return res;
}

glm::vec2 parseVec2(const std::string& line, std::string::size_type begin) {
	std::string::size_type pos = begin, diff;
	glm::vec2 res;
	for(int i = 0; i < 2; ++i) 
	{
		float val = std::stof(line.substr(pos), &diff);
		if ( std::fabs(val) == 0 ) val = 0;
		res[i] = val;
		pos += diff;
	}
	return res;
}

utils::MeshData::FaceData mapIndices(const utils::MeshData& data, const utils::MeshData::FaceData& f) {
	utils::MeshData::FaceData res;

	int idx;
	for ( int i = 0; i < 3; ++i) {
		idx = f.indices[i].positionIdx;
		if (idx < 0) 
		{
			idx = data.positions.size() + idx; 
		} else {
			--idx;
		}
		res.indices[i].positionIdx = idx;	

		if (f.indices[i].textureCoordinateIdx) 
		{
			idx = f.indices[i].textureCoordinateIdx.value();
			if (idx < 0)
			{
				idx = data.textureCoordinates.size() + idx;
			} else {
				--idx;
			}
			res.indices[i].textureCoordinateIdx = idx;
		} else {
			res.indices[i].textureCoordinateIdx = std::nullopt;
		}

		if (f.indices[i].normalIdx)
		{
			idx = f.indices[i].normalIdx.value();
			if (idx < 0)
			{
				idx = data.normals.size() + idx;
			} else {
				--idx;
			}
			res.indices[i].normalIdx = idx;
		} else {
			res.indices[i].normalIdx = std::nullopt;
		}
	}

	return res;
}


struct Command 
{ 
	static bool check(const std::string_view& name) { return name == "#"; }
	static void parse(
			const std::string& line,
			std::string::size_type begin,
			utils::MeshData& mesh) {}
};

struct Vertex
{
	static bool check(const std::string_view& name) { return name == "v"; }
	static void parse(
			const std::string& line,
			std::string::size_type begin,
			utils::MeshData& mesh) 
	{
		mesh.positions.emplace_back(parseVec3(line, begin));
	}
};

struct TextureCoordinate
{
	static bool check(const std::string_view& name) { return name == "vt"; } 
	static void parse(
			const std::string& line,
			std::string::size_type begin,
			utils::MeshData& mesh) 
	{
		mesh.textureCoordinates.emplace_back(parseVec2(line, begin));
	}
};

struct Normal
{
	static bool check(const std::string_view& name) { return name == "vn"; }
	static void parse(
			const std::string& line,
			std::string::size_type begin,
			utils::MeshData& mesh) 
	{
		mesh.normals.emplace_back(parseVec3(line, begin));
	}
};

struct Face
{
	static bool check(const std::string_view& name) { return name == "f"; }
	static void parse(
			const std::string& line,
			std::string::size_type begin,
			utils::MeshData& mesh) 
	{
		utils::MeshData::FaceData f;
		std::string::size_type pos = begin, diff;
		for( auto& v : f.indices ) {
			v.positionIdx = std::stoi(line.substr(pos), &diff); 		
			pos += diff;
			if (line[pos] == '/')  
			{
				++pos;
				if (line[pos] == '/') // with normal, without texture
				{
					++pos;
					v.normalIdx = std::stoi(line.substr(pos), &diff);
					v.textureCoordinateIdx = std::nullopt;
					pos += diff;
				}
				else // with texture
				{
					v.textureCoordinateIdx = std::stoi(line.substr(pos), &diff);
					pos += diff;
					if (line[pos] == '/') // with normal
					{
						++pos;
						v.normalIdx = std::stof(line.substr(pos), &diff);
						pos += diff;
					}
					else // without normal
					{
						v.normalIdx = std::nullopt;
					}
				}
			} 
			else // only position 
			{
				v.textureCoordinateIdx = std::nullopt;
				v.normalIdx = std::nullopt;
			}
		} 
		if (
				f.indices[0].normalIdx.has_value()
				!= f.indices[1].normalIdx.has_value()
				||
				f.indices[1].normalIdx.has_value()
				!= f.indices[2].normalIdx.has_value()
				||
				f.indices[0].textureCoordinateIdx.has_value()
				!= f.indices[1].textureCoordinateIdx.has_value()
				||
				f.indices[1].textureCoordinateIdx.has_value()
				!= f.indices[2].textureCoordinateIdx.has_value())
		{
			throw parsing_error("face with inconsistent vertex descriptions");	
		}
		mesh.faces.emplace_back(mapIndices(mesh, f));
	}
};

struct MtlLib {
	static bool check(const std::string_view& name) { return name == "mtllib"; }
	static void parse(
			const std::string& line,
			std::string::size_type begin,
			utils::MeshData& mesh) {
			spdlog::warn("skip parsing of marital!");
	}
};

struct UseMtl {
	static bool check(const std::string_view& name) { return name == "usemtl"; }
	static void parse(
			const std::string& line,
			std::string::size_type begin,
			utils::MeshData& mesh) {}
};

struct SmoothGroup {
	static bool check(const std::string_view& name) { return name == "s"; }
	static void parse(
			const std::string& line,
			std::string::size_type begin,
			utils::MeshData& mesh) {}
};

using LineTypes = std::tuple<
	Command,
	Vertex,
	TextureCoordinate,
	Normal,
	Face,
	MtlLib,
	UseMtl,
	SmoothGroup> ;
template<int I = 0>
void parseLine(
		const std::string_view& name, 
		const std::string& line,
		std::string::size_type begin,
		utils::MeshData& mesh) 
{
	using LineType = std::tuple_element_t<I, LineTypes>;
	if (LineType::check(name)) {
		LineType::parse(line, begin, mesh);	
	}
	else
	{
		if constexpr (I + 1 < std::tuple_size_v<LineTypes>)
			parseLine<I + 1>(name, line, begin, mesh);
		else
			throw parsing_error("unknown line type: >" + std::string(name) + "<");
	}
}

void parseLine( const std::string& line, utils::MeshData& data) {
	std::string::size_type begin = 0, end = 0;
	while( std::isspace( line[begin] ) ) { ++begin; }

	end = line.find_first_of(' ', begin);

	if ( end == std::string::npos) 
	{
		if (line.length() != 0 &&  (line.length() != 1 || !std::isspace(line[0])))
		{
			throw parsing_error("expected space after type identifier!");
		}
	}
	else
	{
		parseLine(
				{line.c_str() + begin, end - begin}, 
				line.substr(begin), end, data);
	}
}

void parseObj( std::ifstream& file, utils::MeshData& data) {
	std::string line, type;
	int lineNumber = 0;

	try {
	while ( std::getline(file, line) ) 
	{
		parseLine(line, data);
		++lineNumber;
	}
	} catch (const parsing_error& err) {
		throw parsing_error("line: " + std::to_string(lineNumber)
				+ ": "+ err.what());
	}
}

namespace utils {
	MeshData::Handle MeshData::load( const char* _fileName ) 
	{
		MeshData* data = new MeshData;
		std::string_view suffix( _fileName );
		auto pos = suffix.find_last_of( '.' );
		suffix.remove_prefix( pos );
		if ( suffix == ".obj" ) {
			std::ifstream file( _fileName );
			if (file) {
				try {
					parseObj( file, *data );
				} catch (const parsing_error& err) {
					spdlog::error("failed to load mesh data from file: '{}' with: '{}'",
							_fileName, err.what());
				}
			} else {
				spdlog::error("failed to open file: '{}'", _fileName);
			}
		}  else {
			errorTypeNotSupported( _fileName );
		}

		return data;
	}

	void MeshData::unload( MeshData::Handle _meshData ) 
	{
		delete const_cast<MeshData*>( _meshData );
	}
} // end utils
