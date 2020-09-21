#pragma once

#include "../../graphics/renderer/sprite.hpp"
#include "../core/component.hpp"
#include <glm/glm.hpp>
#include <string>

// todo move this to a better location and namespace
using Color = glm::vec4;
using Alignment = glm::vec2;

namespace game { namespace components{

	struct Sprite : public MultiComponent
	{
		/// @brief Create a sprite component.
		/// @param [in] _size The size of the sprite in pixels. 
		///		(0,0): Use the original size of the underlying _sprite.
		Sprite(const graphics::Sprite& _sprite, 
			const glm::vec3& _pos = glm::vec3(0.f), 
			const glm::vec2 _size = glm::vec2(0.f),
			float _rotation = 0.f)
			: sprite(&_sprite), position(_pos), rotation(_rotation), tile(0),
			scale(_size == glm::vec2(0.f) ? glm::vec2(1.f) : _size / glm::vec2(_sprite.size))
		{}

		glm::vec3 position; // z for depth buffer
		glm::vec2 scale;
		float rotation;
		glm::u16vec2 tile;
		const graphics::Sprite* sprite;
	};

	struct Label
	{
		Label(const std::string& _text = "", 
			float _fontSize = 10.f,
			const glm::vec3& _pos = glm::vec3(0.f, 0.f, -0.5f),
			const Color& _color = Color(1.f),
			const Alignment& _alignment = Alignment(0.f, 0.f),
			float _rotation = 0.f,
			bool _roundToPixel = false)
			: text(_text), fontSize(_fontSize), position(_pos), color(_color), 
			rotation(_rotation), alignment(_alignment),
			roundToPixel(_roundToPixel)
		{}

		std::string text;
		float fontSize;
		glm::vec3 position;
		Color color;
		float rotation;
		Alignment alignment;
		bool roundToPixel;
	};

	struct Position2D
	{
		Position2D(const glm::vec2 v) : value(v) {}
		glm::vec2 value;
	};

	struct Velocity2D
	{
		Velocity2D(const glm::vec2 v) : value(v) {}
		glm::vec2 value;
	};

	struct Rotation2D
	{
		Rotation2D(float v) : value(v) {}
		float value;
	};

	struct Scale2D
	{
		Scale2D(const glm::vec2 v) : value(v){}
		glm::vec2 value;
	};

	struct Transform2D
	{
		Transform2D(const glm::mat3& m) : value{ m } {}
		glm::mat3 value;
	};
} }