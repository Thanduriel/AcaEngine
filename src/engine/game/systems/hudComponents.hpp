#pragma once

#include "../../graphics/renderer/sprite.hpp"
#include "../../graphics/core/texture.hpp"
#include "../core/entity.hpp"
#include "../core/component.hpp"
#include "../../math/geometrictypes.hpp"
#include "../../utils/alignment.hpp"

#include <memory>
#include <functional>

namespace game{ 
namespace components{

	// controls position relative to a Parent's BoundingRectangle
	struct Anchor
	{
		utils::Alignment alignment;
	};

	struct BoundingRectangle
	{
		BoundingRectangle(glm::vec2 _size = {}, glm::vec2 _align = {})
			: size(_size), alignment(_align)
		{}

		BoundingRectangle(const math::Rectangle& _bounds)
			: size(_bounds.size()),
			alignment(glm::abs(_bounds.min) / size)
		{}

		glm::vec2 size;
		utils::Alignment alignment;
	};

	struct BoundingRectangleNeedsUpdate {};

	struct FillBar
	{
		// Same parameters as graphics::Sprite
		FillBar(float _alignX, float _alignY,
			const graphics::Texture2D& _background,
			const graphics::Texture2D& _fillTexture, 
			int _texX = 0, int _texY = 0, 
			int _texWidth = -1, int _texHeight = -1 );

		void setFillRatio(float _fillRatio);

		std::unique_ptr<graphics::Sprite> sprite;
		std::unique_ptr<graphics::Sprite> backgroundSprite;
		int initialSize;
	};

	struct Button
	{
		template<typename Fn>
		Button(Fn _fn) : onClick(_fn) {}

		std::function<void()> onClick;
	};

	// positions all its childs automatically
	struct AutoArrange
	{
	};
} }