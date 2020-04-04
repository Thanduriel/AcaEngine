#pragma once

#include "game/core/entity.hpp"
#include "game/core/action.hpp"
#include "game/components/simpleComponents.hpp"

namespace game {
namespace operations {

	class UpdateTransformPosition
	{
	public:
		void operator()(components::Transform& _transform, const components::Position& _position) const;
	};
}}
