#include "drawSprites.hpp"
#include "../../graphics/renderer/spriterenderer.hpp"

namespace game { namespace operations {

	void DrawSprites::operator()(const components::Sprite& _sprite, const components::Position& _position) const
	{
		m_renderer.draw(*_sprite.sprite, 
			_position.value + glm::vec3(_sprite.transform.position, _sprite.depth), 
			_sprite.transform.rotation, _sprite.transform.scale);
	}

	void DrawSprites2D::operator()(const components::Sprite& _sprite,
		const components::Transform2D& _transform) const
	{
		const components::Transform2D transform = _sprite.transform * _transform;

		m_renderer.draw(*_sprite.sprite,
			glm::vec3(transform.position, _sprite.depth),
			transform.rotation, transform.scale,
			static_cast<float>(_sprite.tile.x), static_cast<float>(_sprite.tile.y));
	}
}}