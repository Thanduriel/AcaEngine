#pragma once

#include "utils/containers/slotmap.hpp"
#include "utils/metaProgHelpers.hpp"
#include "action.hpp"
#include "entity.hpp"
#include "component.hpp"
#include <tuple>
#include <utility>
#include <type_traits>

namespace game {

	template<component_type... Components>
	class Registry
	{
		template<typename Val, bool MultiSlot>
		class SlotMapDecider {};

		template<typename Val>
		class SlotMapDecider<Val, false> : public utils::SlotMap<Entity::BaseType, Val> {};

		template<typename Val>
		class SlotMapDecider<Val, true> : public utils::MultiSlotMap<Entity::BaseType, Val> {};

		template<typename Val>
		using SM = SlotMapDecider < Val, std::is_base_of_v<MultiComponent, Val>>;
	public:
		Entity create()
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

		void erase(Entity _ent)
		{
			removeHelper<void, Components...>(_ent);

			m_unusedEntities.push_back(_ent);
			m_generations[_ent.toIndex()].entity = INVALID_ENTITY;
		}

		template<component_type Component, typename... Args>
		void addComponent(Entity _ent, Args&&... _args)
		{
			getContainer<Component>().emplace(_ent.toIndex(), std::forward<Args>(_args)...);
		}

		template<component_type Component>
		bool hasComponent(Entity _ent) const { return getContainer<Component>().contains(_ent); }

		// Retrieve a component associated with an entity.
		// Does not check whether it exits.
		template<component_type Component>
		Component& getComponent(Entity _ent) { getContainer<Component>()[_ent]; }
		template<component_type Component>
		const Component& getComponent(Entity _ent) const { getContainer<Component>()[_ent]; }

		// Execute an Action on all entities having the components
		// expected by Action::operator(...).
		template<typename Action>
		void execute(Action& _action) { executeUnpack(_action, utils::UnpackFunction(&Action::operator())); }
		// This explicit version is only needed to capture rvalues.
		template<typename Action>
		void execute(const Action& _action) { executeUnpack(_action, utils::UnpackFunction(&Action::operator())); }

		struct EntityRef
		{
			Entity entity;
			unsigned generation;
		};

		EntityRef getRef(Entity _ent) const { return m_generations[_ent.toIndex()]; }

		bool isValid(Entity _ent) const { return m_generations[_ent.toIndex()].entity == _ent; }
		bool isValid(EntityRef _ent) const 
		{
			const EntityRef& ref = m_generations[_ent.toIndex()];
			return ref.entity == _ent.entity && _ent.generation == ref.generation;
		}
	private:
		template<typename Action, typename Comp, typename... Comps>
		void executeUnpack(Action& _action, utils::UnpackFunction<std::remove_cv_t<Action>, Comp, Comps...>)
		{
			if constexpr (std::is_convertible_v<Comp,Entity>)
				executeImpl<true, Action, std::decay_t<Comps>...>(_action);
			else
				executeImpl<false, Action, std::decay_t<Comp>, std::decay_t<Comps>...>(_action);
		}

		template<bool WithEnt, typename Action, component_type Comp, component_type... Comps>
		void executeImpl(Action& _action)
		{
			auto& mainContainer = std::get<SM<Comp>>(m_components);
			for (auto it = mainContainer.begin(); it != mainContainer.end(); ++it)
			{
				executeHelper<WithEnt, Comps...>(_action, Entity(it.key()), it.value());
			}
		
		}

		template<bool WithEntity, component_type Comp, component_type... Comps, typename Action, typename... Args>
		void executeHelper(Action& _action, Entity _ent, Args&... _args)
		{	
			auto& comps = std::get<SM<Comp>>(m_components);
			if(comps.contains(_ent.toIndex()))
				executeHelper<WithEntity, Comps...>(_action, _ent, _args..., comps[_ent.toIndex()]);
		}

		template<bool WithEntity, typename Action, typename... Args>
		void executeHelper(Action& _action, Entity _ent,  Args&... _args)
		{
			if constexpr (WithEntity)
				_action(_ent, _args...);
			else
				_action(_args...);
		}

		template<typename Dummy, typename Comp, typename... Comps>
		void removeHelper(Entity _ent)
		{
			auto& comps = std::get<SM<Comp>>(m_components);
			if (comps.contains(_ent.toIndex()))
				comps.erase(_ent.toIndex());
			removeHelper<void, Comps...>(_ent);
		}

		template<typename Dummy>
		void removeHelper(Entity _ent) {}

		template<component_type Comp>
		SM<Comp>& getContainer() { return std::get<SM<Comp>>(m_components); }

		template<component_type Comp>
		const SM<Comp>& getContainer() const { return std::get<SM<Comp>>(m_components); }

		std::vector<Entity> m_unusedEntities;
		uint32_t m_maxNumEntities = 0u;
		std::tuple<SM<Components>...> m_components;
		std::vector<EntityRef> m_generations;
	};
}
