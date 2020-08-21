#pragma once

#include <engine/graphics/renderer/spriterenderer.hpp>
#include <engine/graphics/core/texture.hpp>

namespace game{ 
namespace components{

	struct BoundingRectangle
	{
		BoundingRectangle(glm::vec2 _size, glm::vec2 _center) : size(_size), center(_center) {}

		glm::vec2 size;
		glm::vec2 center;	// relative [0,1]
	};

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

} }