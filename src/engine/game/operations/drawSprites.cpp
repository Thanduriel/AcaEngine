#include "drawSprites.hpp"

namespace game { namespace operations {

	void DrawSprites::operator()(const components::Sprite& _sprite, const components::Position& _position) const
	{
		m_renderer.draw(*_sprite.sprite, 
			_position.value + _sprite.position, 
			_sprite.rotation, _sprite.scale);
	}

	void DrawSprites2D::operator()(const components::Sprite& _sprite,
		const components::Position2D& _position,
		const components::Rotation2D& _rotation) const
	{
		m_renderer.draw(*_sprite.sprite,
			glm::vec3(_position.value, 0.f) + _sprite.position,
			_rotation.value + _sprite.rotation, _sprite.scale);
	}
}}