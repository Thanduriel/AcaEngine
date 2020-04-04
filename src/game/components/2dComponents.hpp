#pragma once

#include "graphics/renderer/spriterenderer.hpp"
#include "game/core/component.hpp"
#include <glm/glm.hpp>

namespace game { namespace components{

	struct Sprite : public MultiComponent
	{
		glm::vec2 position;
		glm::vec2 scale;
		float rotation;
		const graphics::Sprite* sprite;
	};

} }