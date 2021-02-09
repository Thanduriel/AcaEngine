#include "hudActors.hpp"
#include "../../game/core/lifetimeManager2.hpp"
#include "../../game/components/simpleComponents.hpp"

namespace game {

	namespace comp = components;

	FillBar::FillBar(ComponentCreator& _creator, glm::vec2 _position,
		float _alignX, float _alignY,
		const graphics::Texture2D& _backgroundTex,
		const graphics::Texture2D& _fillTex,
		int _texX, int _texY,
		int _texWidth, int _texHeight)
	{
		_creator.add<components::Transform2D>(_position);
		auto& fillBar = _creator.addR<components::FillBar>(_alignX, _alignY, _backgroundTex, _fillTex,
			_texX, _texY, _texWidth, _texHeight);
		_creator.add<components::Sprite>(*fillBar.sprite);
		_creator.add<components::BoundingRectangle>(fillBar.backgroundSprite->size, glm::vec2(_alignX, _alignY));
		_creator.child()
			.add<components::Transform2D>(_position)
			.add<components::Sprite>(*fillBar.backgroundSprite);
	}

	Label::Label(ComponentCreator& _creator, const std::string& _text,
		float _fontSize,
		const glm::vec3& _pos,
		const Color& _color,
		const Alignment& _alignment,
		float _rotation)
	{
		_creator.add<components::Transform2D>(glm::vec2(_pos.x, _pos.y));
		_creator.add<components::Label>(_text, _fontSize, glm::vec3(0.f, 0.f, _pos.z), _color, _alignment, _rotation);
	}
}