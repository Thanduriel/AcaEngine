#pragma once

#include "../../utils/containers/slotmap.hpp"
#include "../../utils/metaProgHelpers.hpp"
#include "entity.hpp"
#include "component.hpp"
#include <tuple>
#include <utility>
#include <type_traits>
#include <optional>

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
		// Make a new entity managed by this registry.
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

		class ComponentCreator
		{
		public:
			ComponentCreator(Registry& _registry, Entity _ent)
				: entity(_ent), m_registry(_registry)
			{}

			template<component_type Component, typename... Args>
			Component& addComponent(Args&&... _args) const
			{
				return m_registry.addComponent<Component>(entity, std::forward<Args>(_args)...);
			}

			const Entity entity;
		private:
			Registry& m_registry;
		};
		template<typename Actor, typename... Args>
		Entity create(Args&&... _args)
		{
			Entity ent = create();
			ComponentCreator creator(*this, ent);
			Actor act(creator, std::forward<Args>(_args)...);
			return ent;
		}

		// Remove an entity with all its components.
		void erase(Entity _ent)
		{
			ASSERT(m_generations[_ent.toIndex()].entity != INVALID_ENTITY, "Attempting to erase a non existent entity.");

			removeHelper<void, Components...>(_ent);

			m_unusedEntities.push_back(_ent);
			m_generations[_ent.toIndex()].entity = INVALID_ENTITY;
		}

		// Add a new component to an existing entity. No changes are done if Component is
		// not a MultiComponent and _ent already has a component of this type.
		// @return A reference to the new component or the already existing component.
		template<component_type Component, typename... Args>
		Component& addComponent(Entity _ent, Args&&... _args)
		{
			return getContainer<Component>().emplace(_ent.toIndex(), std::forward<Args>(_args)...);
		}

		// Remove all components of the specified type.
		template<component_type Component>
		void clearComponent()
		{
			getContainer<Component>().clear();
		}

		template<component_type Component>
		bool hasComponent(Entity _ent) const { return getContainer<Component>().contains(_ent.toIndex()); }

		// Retrieve a component associated with an entity.
		// Does not check whether it exits.
		template<component_type Component>
		Component& getComponent(Entity _ent) { return getContainer<Component>()[_ent.toIndex()]; }
		template<component_type Component>
		const Component& getComponent(Entity _ent) const { return getContainer<Component>()[_ent.toIndex()]; }

		// Execute an Action on all entities having the components
		// expected by Action::operator(...).
		template<typename Action>
		void execute(Action& _action) { executeUnpack(_action, utils::UnpackFunction(&Action::operator())); }
		// This explicit version is only needed to capture rvalues.
		template<typename Action>
		void execute(const Action& _action) { executeUnpack(_action, utils::UnpackFunction(&Action::operator())); }

		// Basically a weak pointer to an Entity.
		struct EntityRef
		{
			EntityRef() : entity(INVALID_ENTITY), generation(0) {}

		private:
			EntityRef(Entity _ent, unsigned _generation)
				: entity(_ent), generation(_generation)
			{}

			friend class Registry;

			Entity entity;
			unsigned generation;
		};

		EntityRef getRef(Entity _ent) const { return m_generations[_ent.toIndex()]; }

		// Checks whether an entity is managed by this registry.
		// An Entity can be valid even if previously deleted, if the id was reassigned.
		// Use an EntityRef instead to prevent this possibility.
		bool isValid(Entity _ent) const { return _ent && m_generations[_ent.toIndex()].entity == _ent; }

		// Attempt to retrieve the referenced entity.
		// @return The entity or an INVALID_ENTITY if the ref is not valid.
		Entity getEntity(EntityRef _ent) const
		{
			const EntityRef& ref = m_generations[_ent.entity.toIndex()];
			if (ref.entity == _ent.entity && _ent.generation == ref.generation)
				return _ent.entity;
			else return INVALID_ENTITY;
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
