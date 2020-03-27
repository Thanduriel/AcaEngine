#pragma once

#include "entity.hpp"
#include <vector>

namespace game {

	// Handles ordered construction and deletion of entities.
	class LifetimeManager
	{
	public:

		void destroy(Entity _ent) { m_deleted.push_back(_ent); }

		template<typename Registry>
		void cleanup(Registry& _registry)
		{
			for (Entity ent : m_deleted)
			{
				// prevent double deletes
				if (_registry.isValid(ent)) _registry.erase(ent);
			}

			m_deleted.clear();
		}
	private:
	//	utils::SlotMap<Entity, unsigned> m_generation;
		std::vector<Entity> m_deleted;
	};
}