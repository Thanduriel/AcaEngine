#pragma once

#include <concepts>

namespace game {

	// Inherit from this to allow multiple components of this type attached to the same entity.
	class MultiComponent
	{};

	// Requirements that a component type needs to fullfill.
	template<class T>
	concept component_type = std::movable<T>;
}