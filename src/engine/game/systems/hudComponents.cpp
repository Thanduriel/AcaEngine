#include "hudComponents.hpp"

namespace game {
namespace components{

	FillBar::FillBar(float _alignX, float _alignY,
		const graphics::Texture2D& _textureHandle,
		int _texX, int _texY,
		int _texWidth, int _texHeight)
		: sprite(std::make_unique<graphics::Sprite>(_alignX, _alignY, 
			&_textureHandle, _texX, _texY, _texWidth, _texHeight)),
		initialSize(sprite->size.y)
	{}

	void FillBar::setFillRatio(float _fillRatio)
	{
		sprite->size.y = static_cast<int>(_fillRatio * initialSize);
	}
}}