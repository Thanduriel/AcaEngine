#pragma once

#include "../core/lifetimeManager2.hpp"
#include "../components/components2D.hpp"
#include "hudComponents.hpp"

namespace game {

	class FillBar
	{
	public:
		FillBar(ComponentCreator& _creator, glm::vec2 _position,
			float _alignX, float _alignY,
			const graphics::Texture2D& _backgroundTex,
			const graphics::Texture2D& _fillTex,
			int _texX = 0, int _texY = 0,
			int _texWidth = -1, int _texHeight = -1);
	};

	class Label
	{
	public:
		Label(ComponentCreator& _creator, const std::string& _text = "",
			float _fontSize = 10.f,
			const glm::vec3& _pos = glm::vec3(0.f, 0.f, -0.5f),
			const Color& _color = Color(1.f),
			const Alignment& _alignment = Alignment(0.f, 0.f),
			float _rotation = 0.f);
	};
}