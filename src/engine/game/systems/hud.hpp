#pragma once

#include "hudComponents.hpp"
#include "../core/registry2.hpp"
#include "../core/lifetimeManager2.hpp"
#include "../components/components2D.hpp"
#include "../../graphics/camera.hpp"
#include "../../graphics/renderer/spriterenderer.hpp"

namespace graphics {
	class FontRenderer;
}

namespace game {

	class Hud
	{
	public:
		Hud(graphics::FontRenderer* _fontRenderer = nullptr);

		void draw();
		void processInputs();
	protected:
		// Gives the position inside an entity with a BoundingRectangle in screen space.
		glm::vec2 getAbsolutePosition(Entity _entity, glm::vec2 _relativePosition) const;
		
		// Creates an activates a custom cursor for this hud.
		void createCursor(const graphics::Sprite& _sprite);

		using HudRegistry = Registry2;
		using HudManager = LifetimeManager2;

		HudRegistry m_registry;
		HudManager m_manager;

		Entity m_this; // Hud Entity to be used as parent
		Entity m_cursor; // Cursor entity is invalid until createCursor was called
		graphics::SpriteRenderer m_spriteRenderer;
		graphics::FontRenderer* m_fontRenderer;
		graphics::Camera m_camera;
	};
}