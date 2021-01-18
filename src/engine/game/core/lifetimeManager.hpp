#pragma once

#include "registry.hpp"
#include <vector>

namespace game {

	// Handles ordered construction and deletion of entities.
	template<component_type... Components>
	class LifetimeManager
	{
	public:
		LifetimeManager(Registry<Components...>& _registry) : m_registry(_registry) {}

		Entity create()
		{
			return m_registry.create();
		}

		void destroy(Entity _ent) { m_deleted.push_back(_ent); }

		template<component_type Component, typename... Args>
		void addComponent(Entity _ent, Args&&... _args)
		{
			getContainer<Component>().emplace_back(_ent, Component{ std::forward<Args>(_args)... });
		}

		// Move newly created components into the registry.
		void moveComponents()
		{
			moveComponentsImpl<void, Components...>();
		}

		// Remove marked entities.
		void cleanup()
		{
			for (Entity ent : m_deleted)
			{
				// prevent double deletes
				if (m_registry.isValid(ent)) m_registry.erase(ent);
			}

			m_deleted.clear();
		}
	private:
		template<typename Comp>
		std::vector<std::pair<Entity, Comp>>& getContainer()
		{
			return std::get< std::vector<std::pair<Entity, Comp>>>(m_newComponents);
		}

		template<typename Dummy, typename Comp, typename... Comps>
		void moveComponentsImpl()
		{
			for (auto& [ent, comp] : getContainer<Comp>())
				m_registry.template addComponent<Comp>(ent, std::move(comp));
			getContainer<Comp>().clear();

			moveComponentsImpl<Dummy, Comps...>();
		}

		template<typename Dummy>
		void moveComponentsImpl(){}

		Registry<Components...>& m_registry;
		std::vector<Entity> m_deleted;
		std::tuple<std::vector<std::pair<Entity, Components>>...> m_newComponents;
	};
}
