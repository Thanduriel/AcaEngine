#pragma once

#include <limits>
#include <cstdint>

namespace game {
	struct Entity
	{
		using BaseType = uint32_t;

		constexpr explicit Entity(BaseType id) : m_id(id) {}
		Entity() = default;

		explicit operator BaseType() const { return m_id; }
		BaseType toIndex() const { return m_id; }

		constexpr Entity operator=(Entity oth) { m_id = oth.m_id; return *this; }
		constexpr bool operator==(Entity oth) const { return m_id == oth.m_id; }
	private:
		BaseType m_id;
	};

	constexpr static Entity INVALID_ENTITY = Entity(std::numeric_limits<Entity::BaseType>::max());
}