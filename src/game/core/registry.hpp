#pragma once

#include "utils/containers/slotmap.hpp"
#include "action.hpp"
#include "entity.hpp"
#include <tuple>
#include <utility>

namespace game {

	template<typename... Components>
	class Registry
	{
		template<typename Val>
		using SM = utils::SlotMap<Entity, Val>;
	public:
		Entity create()
		{
			if (m_unusedEntities.size())
			{
				Entity e = m_unusedEntities.back();
				m_unusedEntities.pop_back();
				return e;
			}
			else return m_maxNumEntities++;
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

	private:
		template<typename Comp, typename... Comps, typename CurMin>
		auto findMinContainer(CurMin& _curMin, std::size_t _minSize)
		{
			auto& comps = std::get<SM<Comp>>(m_components);
			return comps.size() < _minSize ?
				smallestContainer<Comps...>(comps, comps.size())
				: smallestContainer<Comps>(_curMin, _minSize);
		}

		template<typename CurMin>
		auto findMinContainer(CurMin& _curMin, std::size_t _minSize)
		{
			return _curMin;
		}

		template<typename Comp, typename... Comps, typename Action, typename... Args>
		void executeHelper(Action& _action, Entity _ent, Args&... _args)
		{	
			auto& comps = std::get<SM<Comp>>(m_components);
			if(comps.contains(_ent))
				executeHelper<Comps...>(_action, _ent, _args..., comps[_ent]);
		}

		template<typename Action, typename... Args>
		void executeHelper(Action& _action, Entity _ent, Args&... _args)
		{
			_action(_ent, _args...);
		}

		std::vector<Entity> m_unusedEntities;
		uint32_t m_maxNumEntities = 0;
		std::tuple<utils::SlotMap<Entity, Components>...> m_components;
	};
}