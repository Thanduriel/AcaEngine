#include "registry2.hpp"

namespace game {

	Entity Registry2::create()
	{
		Entity ent;
		if (!m_unusedEntities.empty())
		{
			ent = m_unusedEntities.back();
			m_unusedEntities.pop_back();

			m_generations[ent.toIndex()].entity = ent;
			m_generations[ent.toIndex()].generation++;
		}
		else
		{
			ent = Entity(m_maxNumEntities++);
			m_generations.push_back({ ent,0 });
		}
		return ent;
	}

	void Registry2::erase(Entity _ent)
	{
		ASSERT(m_generations[_ent.toIndex()].entity != INVALID_ENTITY, "Attempting to erase a non existent entity.");

		// mark invalid before removing components so that Children removal is simpler
		m_generations[_ent.toIndex()].entity = INVALID_ENTITY;

		removeComponent<components::Children>(_ent);
		removeComponent<components::Parent>(_ent);

		for (auto& components : std::get<StorageMap<DataStorage>>(m_components))
			if (components.data().contains(_ent.toIndex()))
				components.data().erase(_ent.toIndex());

		m_unusedEntities.push_back(_ent);
	}
}
