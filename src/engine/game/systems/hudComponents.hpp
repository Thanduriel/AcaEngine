#pragma once

#include <engine/graphics/renderer/spriterenderer.hpp>
#include <engine/graphics/core/texture.hpp>

namespace game{ 
namespace components{

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

} }