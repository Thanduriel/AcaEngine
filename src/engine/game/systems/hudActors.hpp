#pragma once

#include "../components/components2D.hpp"
#include "hudComponents.hpp"

namespace game {

	class FillBar
	{
	public:
		template< class Creator>
		FillBar(Creator& _creator, glm::vec2 _position,
			float _alignX, float _alignY,
			const graphics::Texture2D& _backgroundTex, 
			const graphics::Texture2D& _fillTex,
			int _texX = 0, int _texY = 0,
			int _texWidth = -1, int _texHeight = -1)
		{
			_creator.addComponent<components::Position2D>(_position);
			_creator.addComponent<components::Rotation2D>(0.f);
			auto& fillBar = _creator.addComponent<components::FillBar>(_alignX, _alignY, _backgroundTex, _fillTex,
				_texX, _texY, _texWidth, _texHeight);
			_creator.addComponent<components::Sprite>(*fillBar.sprite);
			_creator.addComponent<components::Sprite>(*fillBar.backgroundSprite);
			_creator.addComponent<components::BoundingRectangle>(fillBar.backgroundSprite->size, glm::vec2(_alignX, _alignY));
		}
	};
}