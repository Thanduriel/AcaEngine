#pragma once

#include "../../graphics/renderer/sprite.hpp"
#include "../core/component.hpp"
#include "../../math/glmext.hpp"
#include "../../utils/alignment.hpp"
#include <string>

// todo move this to a better location and namespace
using Color = glm::vec4;

namespace game { namespace components {

	struct Transform2D
	{
		Transform2D(const glm::vec2& _pos = {}, float _rot = 0.f, const glm::vec2& _scale = glm::vec2(1.f))
			: position(_pos), rotation(_rot), scale(_scale)
		{}

		glm::vec2 position;
		float rotation;
		glm::vec2 scale;

		Transform2D operator*(const Transform2D& oth) const
		{
			const glm::mat2x2 rot = math::rotation(rotation);
			return Transform2D{
				position + rot * oth.position,
				rotation + oth.rotation,
				scale * oth.scale };
		}
	};

	struct Sprite : public MultiComponent
	{
		/// @brief Create a sprite component.
		/// @param [in] _size The size of the sprite in pixels. 
		///		(0,0): Use the original size of the underlying _sprite.
		Sprite(const graphics::Sprite& _sprite, 
			const glm::vec3& _pos = glm::vec3(0.f), 
			const glm::vec2 _size = glm::vec2(0.f),
			float _rotation = 0.f,
			const Color& _color = Color(0.f))
			: sprite(&_sprite), 
			depth(_pos.z),
			transform(_pos, _rotation, _size == glm::vec2(0.f) ? glm::vec2(1.f) : _size / glm::vec2(_sprite.size)), 
			tile(0),
			color(_color)
		{}

		const graphics::Sprite* sprite;
		float depth;
		Transform2D transform;
		glm::u16vec2 tile;
		Color color;
	};

	struct Label
	{
		Label(const std::string& _text = "", 
			float _fontSize = 10.f,
			const glm::vec3& _pos = glm::vec3(0.f, 0.f, -0.5f),
			const Color& _color = Color(1.f),
			const utils::Alignment& _alignment = utils::Alignment(0.f, 0.f),
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
		utils::Alignment alignment;
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
} }