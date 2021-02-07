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
		const components::Transform2D transform = _transform * _sprite.transform;

		// ordering of sprites should be depended on x due to the isometric perspective
		constexpr float DEPTH_SCALE = 1.f / 1000000.f;
		const float depth = std::clamp(_sprite.depth - transform.position.y * DEPTH_SCALE, -1.f, 0.f);

		m_renderer.draw(*_sprite.sprite,
			glm::vec3(transform.position, depth),
			transform.rotation, 
			transform.scale,
			_sprite.color,
			static_cast<float>(_sprite.tile.x), static_cast<float>(_sprite.tile.y));
	}
}}