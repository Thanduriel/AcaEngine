#include "hudActors.hpp"

namespace game {

	namespace comp = components;

	FillBar::FillBar(Registry2::ComponentCreator& _creator, glm::vec2 _position,
		float _alignX, float _alignY,
		const graphics::Texture2D& _backgroundTex,
		const graphics::Texture2D& _fillTex,
		int _texX, int _texY,
		int _texWidth, int _texHeight)
	{
		_creator.add<comp::Position2D>(_position);
		_creator.add<comp::Rotation2D>(0.f);
		auto& fillBar = _creator.add<comp::FillBar>(_alignX, _alignY, _backgroundTex, _fillTex,
			_texX, _texY, _texWidth, _texHeight);
		_creator.add<comp::Sprite>(*fillBar.sprite);
		_creator.add<comp::Sprite>(*fillBar.backgroundSprite);
		_creator.add<comp::BoundingRectangle>(fillBar.backgroundSprite->size, glm::vec2(_alignX, _alignY));
	}

	Label::Label(Registry2::ComponentCreator& _creator, const std::string& _text,
		float _fontSize,
		const glm::vec3& _pos,
		const Color& _color,
		const Alignment& _alignment,
		float _rotation)
	{
		_creator.add<comp::Position2D>(glm::vec2(_pos.x, _pos.y));
		_creator.add<comp::Rotation2D>(0.f);
		_creator.add<comp::Label>(_text, _fontSize, glm::vec3(0.f, 0.f, _pos.z), _color, _alignment, _rotation);
	}
}