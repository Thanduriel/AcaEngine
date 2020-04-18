#pragma once

#include "../core/entity.hpp"
#include "../components/simpleComponents.hpp"
#include "../components/components2D.hpp"

namespace game {
namespace operations {

	class UpdateTransformPosition
	{
	public:
		void operator()(components::Transform& _transform, const components::Position& _position) const;
	};

	class UpdateTransformPosition2D
	{
	public:
		void operator()(components::Transform2D& _transform, const components::Position2D& _position) const;
	};

	class UpdateTransformRotation2D
	{
	public:
		void operator()(components::Transform2D& _transform, const components::Rotation2D& _rotation) const;
	};
}}
