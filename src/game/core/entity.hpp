#pragma once

#include <limits>

namespace game {
	using Entity = unsigned;

	constexpr static Entity INVALID_ENTITY = std::numeric_limits<Entity>::max();
}