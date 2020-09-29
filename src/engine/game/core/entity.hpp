#pragma once

#include <limits>
#include <cstdint>

namespace game {
	struct Entity
	{
		using BaseType = uint32_t;
		constexpr static BaseType INVALID_ID = std::numeric_limits<Entity::BaseType>::max();

		constexpr explicit Entity(BaseType id) noexcept : m_id(id) {}

		constexpr Entity() noexcept : m_id(INVALID_ID) {}

		/* explicit */
		operator BaseType() const { return m_id; }
		BaseType toIndex() const { return m_id; }

		constexpr operator bool() const { return m_id != INVALID_ID; }

		constexpr Entity operator=(Entity oth) noexcept { m_id = oth.m_id; return *this; }
		constexpr bool operator==(Entity oth) const { return m_id == oth.m_id; }
		constexpr bool operator!=(Entity oth) const { return m_id != oth.m_id; }
	private:
		BaseType m_id;
	};

	constexpr static Entity INVALID_ENTITY = Entity(Entity::INVALID_ID);
}