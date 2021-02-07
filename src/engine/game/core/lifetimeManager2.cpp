#include "LifetimeManager2.hpp"

namespace game {

	WeakComponentVector::WeakComponentVector(WeakComponentVector&& _oth) noexcept
		: m_moveToRegistry(_oth.m_moveToRegistry),
		m_clear(_oth.m_clear),
		m_entities(std::move(_oth.m_entities)),
		m_components(std::move(_oth.m_components))
	{
	}

	WeakComponentVector& WeakComponentVector::operator=(WeakComponentVector&& _oth) noexcept
	{
		m_moveToRegistry = _oth.m_moveToRegistry;
		m_clear = _oth.m_clear;
		m_entities = std::move(_oth.m_entities);
		m_components = std::move(_oth.m_components);

		return *this;
	}

	// ************************************************** //
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
		for (auto& it : m_newComponents)
			it.data().moveToRegistry(m_registry);
	}
}