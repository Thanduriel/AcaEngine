#include "drawSprites.hpp"

namespace game { namespace operations {

	void DrawSprites::operator()(const components::Sprite& _sprite, const components::Position& _position) const
	{
		m_renderer.draw(*_sprite.sprite, 
			_position.value + glm::vec3(_sprite.position.x, _sprite.position.y,0.f), 
			_sprite.rotation, _sprite.scale);
	}

}}