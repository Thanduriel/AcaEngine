#include "lifetimeManager2.hpp"

namespace game {

	Entity LifetimeManager2::create()
	{
		return m_registry.create();
	}

	void LifetimeManager2::destroy(Entity _ent)
	{
		m_deleted.push_back(_ent);
	}
}