#pragma once

#include <engine/game/core/componentaccess.hpp>
#include <engine/game/components/simpleComponents.hpp>

namespace game {
namespace systems{

	class Transforms
	{
	public:
		using Components = ComponentTuple<
			ReadAccess<components::Children>,
			ReadAccess<components::Position>,
			ReadAccess<components::Rotation>,
			ReadAccess<components::Scale>,
			ReadAccess<components::Parent>,
			WriteAccess<components::Transform>,
			WriteAccess<components::TransformNeedsUpdate>>;

		void update(Components _comps);
	};
}
}