#pragma once

#include "utils/containers/slotmap.hpp"
#include "action.hpp"
#include "entity.hpp"
#include "component.hpp"
#include <tuple>
#include <utility>
#include <concepts>

namespace game {

	template<typename... Components>
	class Registry
	{
		template<typename Val, bool MultiSlot>
		class SlotMapDecider {};

		template<typename Val>
		class SlotMapDecider<Val, false> : public utils::SlotMap<Entity, Val> {};

		template<typename Val>
		class SlotMapDecider<Val, true> : public utils::MultiSlotMap<Entity, Val> {};

		template<typename Val>
		using SM = SlotMapDecider < Val, std::is_base_of_v<MultiComponent, Val>>;
	public:
		Entity create()
		{
			Entity ent;
			if (m_unusedEntities.size())
			{
				ent = m_unusedEntities.back();
				m_unusedEntities.pop_back();

				m_generations[ent].entity = ent;
				m_generations[ent].generation++;
			}
			else
			{
				ent = m_maxNumEntities++;
				m_generations.push_back({ ent,0 });
			}
			return ent;
		}

		void erase(Entity _ent)
		{
			removeHelper<void, Components...>(_ent);

			m_unusedEntities.push_back(_ent);
			m_generations[_ent].entity = INVALID_ENTITY;
		}

		template<typename Component, typename... Args>
		void addComponent(Entity _ent, Args&&... _args)
		{
			std::get<SM<Component>>(m_components).emplace(_ent, std::forward<Args>(_args)...);
		}

		template<typename None, typename Comp, typename... Comps>
		void execute(Action<None, Comp, Comps...>& _action)
		{
			auto& mainContainer = std::get<SM<Comp>>(m_components);
			for (auto it = mainContainer.begin(); it != mainContainer.end(); ++it)
			{
				executeHelper<Comps...>(_action, it.key(), it.value());
			}
		}

		template<typename None, typename Comp, typename... Comps>
		void executeExt(Action<None, Comp, Comps...>& _action)
		{
			auto& mainContainer = std::get<SM<Comp>>(m_components);
			for (auto it = mainContainer.begin(); it != mainContainer.end(); ++it)
			{
				executeHelperExt<Comps...>(_action, it.key(), it.value());
			}
		}

		struct EntityRef
		{
			Entity entity;
			unsigned generation;
		};

		EntityRef getRef(Entity _ent)
		{
			return m_generations[_ent];
		}

		bool isValid(Entity _ent) const
		{
			return m_generations[_ent].entity == _ent;
		}
		bool isValid(EntityRef _ent) const 
		{
			const EntityRef& ref = m_generations[_ent];
			return ref.entity == _ent.entity && _ent.generation == ref.generation;
		}
	private:

		template<typename Comp, typename... Comps, typename Action, typename... Args>
		void executeHelper(Action& _action, Entity _ent, Args&... _args)
		{	
			auto& comps = std::get<SM<Comp>>(m_components);
			if(comps.contains(_ent))
				executeHelper<Comps...>(_action, _ent, _args..., comps[_ent]);
		}

		template<typename Action, typename... Args>
		void executeHelper(Action& _action, Entity _ent,  Args&... _args)
		{
			_action(_args...);
		}

		template<typename Comp, typename... Comps, typename Action, typename... Args>
		void executeHelperExt(Action& _action, Entity _ent, Args&... _args)
		{
			auto& comps = std::get<SM<Comp>>(m_components);
			if (comps.contains(_ent))
				executeHelper<Comps...>(_action, _ent, _args..., comps[_ent]);
		}

		template<typename Action, typename... Args>
		void executeHelperExt(Action& _action, Entity _ent, Args&... _args)
		{
			_action(_ent, _args...);
		}

		template<typename Dummy, typename Comp, typename... Comps>
		void removeHelper(Entity _ent)
		{
			auto& comps = std::get<SM<Comp>>(m_components);
			if (comps.contains(_ent))
				comps.erase(_ent);
			removeHelper<void, Comps...>(_ent);
		}

		template<typename Dummy>
		void removeHelper(Entity _ent)
		{
		}

		std::vector<Entity> m_unusedEntities;
		uint32_t m_maxNumEntities = 0u;
		std::tuple<SM<Components>...> m_components;
		std::vector<EntityRef> m_generations;
	};
}