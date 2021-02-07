#include "lifetimeManager2.hpp"

namespace game {

	EntityCreator::EntityCreator(Registry2& _registry)
		: m_registry(_registry)
	{
	}

	Entity EntityCreator::create()
	{
		return m_registry.create();
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
		for (WeakComponentVector& container : m_newComponents)
			container.moveToRegistry(m_registry);
	}
}