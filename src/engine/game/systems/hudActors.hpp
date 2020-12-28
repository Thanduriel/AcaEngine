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
			_creator.addComponent<components::Transform2D>(_position);
			auto& fillBar = _creator.addComponent<components::FillBar>(_alignX, _alignY, _backgroundTex, _fillTex,
				_texX, _texY, _texWidth, _texHeight);
			_creator.addComponent<components::Sprite>(*fillBar.sprite);
			_creator.addComponent<components::Sprite>(*fillBar.backgroundSprite);
			_creator.addComponent<components::BoundingRectangle>(fillBar.backgroundSprite->size, glm::vec2(_alignX, _alignY));
		}
	};

	class Label
	{
	public:
		template< class Creator>
		Label(Creator& _creator, const std::string& _text = "",
			float _fontSize = 10.f,
			const glm::vec3& _pos = glm::vec3(0.f, 0.f, -0.5f),
			const Color& _color = Color(1.f),
			const Alignment& _alignment = Alignment(0.f, 0.f),
			float _rotation = 0.f)
		{
			_creator.addComponent<components::Transform2D>(glm::vec2(_pos.x, _pos.y));
			_creator.addComponent<components::Label>(_text, _fontSize, glm::vec3(0.f,0.f, _pos.z), _color, _alignment, _rotation);
		}
	};
}