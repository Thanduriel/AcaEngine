#include "hudComponents.hpp"

namespace game {
namespace components{

	FillBar::FillBar(float _alignX, float _alignY,
		const graphics::Texture2D& _backgroundTexture,
		const graphics::Texture2D& _fillTexture,
		int _texX, int _texY,
		int _texWidth, int _texHeight)
		: sprite(std::make_unique<graphics::Sprite>(_alignX, _alignY, 
			&_fillTexture, _texX, _texY, _texWidth, _texHeight)),
		backgroundSprite(std::make_unique<graphics::Sprite>(_alignX, _alignY,
			&_backgroundTexture, _texX, _texY, _texWidth, _texHeight)),
		initialSize(sprite->size.x)
	{}

	void FillBar::setFillRatio(float _fillRatio)
	{
		const int size = static_cast<int>(_fillRatio * initialSize);
		sprite->data.texCoords.z = uint16_t(_fillRatio * 65535);
		sprite->size.x = size;
	}
}}