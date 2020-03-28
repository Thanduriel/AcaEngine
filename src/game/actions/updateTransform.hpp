#pragma once

#include "game/core/entity.hpp"
#include "game/core/action.hpp"
#include "game/components/simpleComponents.hpp"

namespace game {
namespace actions {

	class UpdateTransformPositionImpl
	{
	public:
		void operator()(Transform& _transform, const Position& _position) const;
	};

	using UpdateTransformPosition = Action<UpdateTransformPositionImpl, Transform, Position>;
}}
