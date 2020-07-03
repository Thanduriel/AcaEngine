#pragma once

#include "hudComponents.hpp"
#include "../core/registry.hpp"
#include "../components/components2D.hpp"
#include "../../graphics/camera.hpp"
#include "../../graphics/renderer/spriterenderer.hpp"

namespace game {

	class Hud
	{
	public:
		Hud();

	protected:
		using HudRegistry = Registry< components::Position2D,
			components::Rotation2D,
			components::Sprite,
			components::FillBar>;

		HudRegistry m_registry;
		graphics::SpriteRenderer m_renderer;
		graphics::Camera m_camera;
	};
}