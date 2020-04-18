#pragma once

#include "../../graphics/renderer/spriterenderer.hpp"
#include "../core/component.hpp"
#include <glm/glm.hpp>

namespace game { namespace components{

	struct Sprite : public MultiComponent
	{
		Sprite(const graphics::Sprite& _sprite, 
			const glm::vec3& _pos = glm::vec3(0.f), 
			const glm::vec2 _scale = glm::vec2(1.f),
			float _rotation = 0.f)
			: sprite(&_sprite), position(_pos), rotation(_rotation), scale(_scale)
		{}

		glm::vec3 position; // z for depth buffer
		glm::vec2 scale;
		float rotation;
		const graphics::Sprite* sprite;
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