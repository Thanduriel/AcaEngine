#pragma once

#include "game/components/components2D.hpp"
#include "game/components/simpleComponents.hpp"

namespace game { namespace operations {

	class DrawSprites
	{
	public:
		DrawSprites(graphics::SpriteRenderer& _renderer) : m_renderer(_renderer){}

		void operator()(const components::Sprite& _sprite, const components::Position& _position) const;

	private:
		graphics::SpriteRenderer& m_renderer;
	};

	class DrawSprites2D
	{
	public:
		DrawSprites2D(graphics::SpriteRenderer& _renderer) : m_renderer(_renderer) {}

		void operator()(const components::Sprite& _sprite, 
			const components::Position2D& _position,
			const components::Rotation2D& _rotation) const;

	private:
		graphics::SpriteRenderer& m_renderer;
	};

} }