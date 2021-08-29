#include "LifetimeManager2.hpp"

namespace game {

	// ************************************************** //
	EntityCreator::EntityCreator(Registry2& _registry)
		: m_registry(_registry)
	{
	}

	Entity EntityCreator::create()
	{
		return m_registry.create();
	}

	Entity EntityCreator::get(const Registry2::EntityRef& _ref) const
	{
		return m_registry.getEntity(_ref);
	}

	Registry2::EntityRef EntityCreator::ref(Entity _ent) const
	{
		return m_registry.getRef(_ent);
	}

	// ************************************************** //
	void EntityDeleter::destroy(Entity _ent)
	{
		m_deleted.push_back(_ent);
	}

	void EntityDeleter::cleanup(Registry2& _registry)
	{
		for (Entity ent : m_deleted)
		{
			// prevent double deletes
			if (_registry.isValid(ent)) _registry.erase(ent);
		}

		m_deleted.clear();
	}

	// ************************************************** //
	void LifetimeManager2::moveComponents()
	{
		for (auto& it : m_newComponents)
			it.data().moveToRegistry(m_registry);
	}
}