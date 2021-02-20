#pragma once

#include <concepts>
#include <type_traits>

namespace game {

	// Inherit from this to allow multiple components of this type attached to the same entity.
	// depriciated! only used by Registry and still buggy
	class MultiComponent{};

	class Message {};

	// Requirements that a component type needs to fulfill.
	template<class T>
	concept component_type = std::movable<T>;

	template<class T>
	concept message_component_type = std::is_base_of_v<Message, T> && component_type<T>;

	template<class T>
	concept data_component_type = !std::is_base_of_v<Message, T> && component_type<T>;
}