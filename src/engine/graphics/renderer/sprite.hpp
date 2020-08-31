#pragma once

#include "../core/texture.hpp"
#include <glm/glm.hpp>

namespace graphics {

	class Sprite
	{
	public:
		/// Create a new sprite definition.
		/// \details Creates a sprite of a certain size and positioning which covers a (region) of a
		///		texture.
		/// \param [in] _alignX Defines which point of the sprite is set to the position when
		///		instances are added.
		///		0: the sprite coordinate is on the left, 0.5: centered in X, 1: on the right side.
		/// \param [in] _alignY Defines which point of the sprite is set to the position when
		///		instances are added.
		///		0: the sprite coordinate is on the bottom, 0.5: centered in Y, 1: on the top side.
		/// \param [in] _textureHandle Bindless handle of a texture.
		/// \param [in] _texX Left pixel coordinate in the texture. The default is 0.
		/// \param [in] _texY Bottom pixel coordinate in the texture. The default is 0.
		/// \param [in] _texWidth The width of the texture region in pixels. The default -1 uses the
		///		full texture.
		/// \param [in] _texWidth The height of the texture region in pixels. The default -1 uses the 
		///		full texture.
		/// \param [in] _numX Number of tiles in X-direction. This can be used for animated sprites.
		/// \param [in] _numX Number of tiles in Y-direction. This can be used for animated sprites.
		Sprite(float _alignX, float _alignY,
			Texture2D::Handle _textureHandle, int _texX = 0, int _texY = 0, int _texWidth = -1, int _texHeight = -1,
			int _numX = 1, int _numY = 1);

#pragma pack(push, 4)
		struct SpriteData
		{
			glm::u16vec4 texCoords;
			uint64_t texture;
			glm::u16vec2 numTiles;
		};
#pragma pack(pop)
		SpriteData data;
		glm::vec2 offset;
		glm::ivec2 size;
	};
}