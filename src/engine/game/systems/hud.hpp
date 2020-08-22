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

		void draw();
		void processInputs();
	protected:
		glm::vec2 getAbsolutePosition(Entity _entity, glm::vec2 _relativePosition) const;
		void createCursor(const graphics::Sprite& _sprite);

		using HudRegistry = Registry< components::Position2D,
			components::Rotation2D,
			components::Sprite,
			components::FillBar,
			components::BoundingRectangle,
			components::Button>;

		HudRegistry m_registry;
		Entity m_this; // Hud Entity to be used as parent
		Entity m_cursor;
		graphics::SpriteRenderer m_spriteRenderer;
		graphics::Camera m_camera;
	};
}