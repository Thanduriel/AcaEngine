#pragma once

#include "game/core/entity.hpp"
#include "game/core/action.hpp"
#include "game/components/simpleComponents.hpp"

namespace game {
namespace actions {

	class UpdateTransformPosition
	{
	public:
		void operator()(Transform& _transform, const Position& _position) const;
	};
}}
