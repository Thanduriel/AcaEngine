#pragma once

#include <limits>
#include <cstdint>
#include <vector>

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

	constexpr static Entity INVALID_ENTITY(Entity::INVALID_ID);

	// Basically a weak pointer to an Entity.
	struct EntityRef
	{
		EntityRef() noexcept : entity(), generation(0) {}

	private:
		EntityRef(Entity _ent, unsigned _generation)
			: entity(_ent), generation(_generation)
		{}

		friend class Registry2;

		Entity entity;
		unsigned generation;
	};

	namespace components {
		// Currently only works correctly together with Position, Rotation, Scale components
		struct Parent
		{
			Parent(Entity ent) : entity(ent) {}

			Entity entity;
		};

		struct Children
		{
			std::vector<Entity> entities;
		};
	}
}