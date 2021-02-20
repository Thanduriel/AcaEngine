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
		auto& fillBar = _creator.addR<comp::FillBar>(_alignX, _alignY, _backgroundTex, _fillTex,
			_texX, _texY, _texWidth, _texHeight);
		_creator.add<comp::Transform2D>(_position)
			.add<comp::Sprite>(*fillBar.sprite)
			.add<comp::BoundingRectangle>(fillBar.backgroundSprite->size, glm::vec2(_alignX, _alignY))
			.child()
				.add<comp::Transform2D>(_position)
				.add<comp::Sprite>(*fillBar.backgroundSprite);
	}

	Label::Label(ComponentCreator& _creator, const std::string& _text,
		float _fontSize,
		const glm::vec3& _pos,
		const Color& _color,
		const utils::Alignment& _alignment,
		float _rotation)
	{
		_creator.add<comp::Transform2D>(glm::vec2(_pos.x, _pos.y))
			.add<comp::Label>(_text, _fontSize, glm::vec3(0.f, 0.f, _pos.z), _color, _alignment, _rotation)
			.add<comp::BoundingRectangle>()
			.add<comp::BoundingRectangleNeedsUpdate>();
	}

	AutoContainer::AutoContainer(ComponentCreator& _creator, glm::vec2 _position)
	{
		_creator.add<comp::Transform2D>(_position)
			.add<comp::BoundingRectangle>()
			.add<comp::AutoArrange>()
			.add<comp::BoundingRectangleNeedsUpdate>();
	}
}