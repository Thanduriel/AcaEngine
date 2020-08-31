#include "sprite.hpp"

namespace graphics {

	using namespace glm;

	Sprite::Sprite(float _alignX, float _alignY,
		Texture2D::Handle _textureHandle, int _texX, int _texY, int _texWidth, int _texHeight,
		int _numX, int _numY)
	{
		// TODO: error handling for parameters
		const float w = _textureHandle ? (float)_textureHandle->getWidth() : 1.0f;
		const float h = _textureHandle ? (float)_textureHandle->getHeight() : 1.0f;

		if (_texWidth == -1) _texWidth = _textureHandle ? _textureHandle->getWidth() : 1;
		if (_texHeight == -1) _texHeight = _textureHandle ? _textureHandle->getHeight() : 1;

		data.texCoords.x = uint16((_texX + 0.0f) / w * 65535);
		data.texCoords.w = uint16((_texY + 0.0f) / h * 65535);
		data.texCoords.z = uint16((_texX - 0.0f + _texWidth) / w * 65535);
		data.texCoords.y = uint16((_texY - 0.0f + _texHeight) / h * 65535);
		data.texture = _textureHandle ? _textureHandle->getGPUHandle() : 0;
		data.numTiles.x = _numX;
		data.numTiles.y = _numY;
		offset.x = -_texWidth * _alignX;
		offset.y = -_texHeight * _alignY;
		size.x = _texWidth;
		size.y = _texHeight;
	}
}