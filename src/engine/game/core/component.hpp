#pragma once

#include <concepts>
#include <type_traits>

namespace game {

	// Inherit from this to allow multiple components of this type attached to the same entity.
	// depreciated! only used by Registry and still buggy
	class MultiComponent{};

	// Inherit from this to create a special component with the following properties:
	// * an entity can possess multiple messages of the same type
	// * no entity wise access, use iterate and clear instead
	class Message {};

	// temporary
	class Flag {};

	// Requirements that any component type needs to fulfill.
	template<class T>
	concept component_type = std::movable<T>;

	// Exclusive concepts that determine the storage type and access patterns.
	template<class T>
	concept message_component_type = component_type<T> && std::is_base_of_v<Message, T>;

	template<class T>
	concept flag_component_type = component_type<T> && std::is_empty_v<T>;

	template<class T>
	concept data_component_type = component_type<T> 
		&& !message_component_type<T> 
		&& !flag_component_type<T>;
}