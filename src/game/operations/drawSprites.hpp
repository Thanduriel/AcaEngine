#pragma once

#include "game/components/2dComponents.hpp"
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

} }