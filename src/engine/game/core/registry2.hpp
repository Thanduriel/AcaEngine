#pragma once

#include "../../utils/containers/slotmap.hpp"
#include "../../utils/containers/weakSlotMap.hpp"
#include "../../utils/metaProgHelpers.hpp"
#include "../../utils/typeIndex.hpp"
#include "entity.hpp"
#include "component.hpp"
#include <tuple>
#include <utility>
#include <type_traits>
#include <optional>
#include <array>

namespace game {
	
	class Registry2
	{
		template<typename Val, bool MultiSlot>
		class SlotMapDecider {};

		template<typename Val>
		class SlotMapDecider<Val, false> : public utils::SlotMap<Entity::BaseType, Val> {};

		template<typename Val>
		class SlotMapDecider<Val, true> : public utils::MultiSlotMap<Entity::BaseType, Val> {};

		template<typename Val>
		using SM = utils::WeakSlotMap<Entity::BaseType>;//SlotMapDecider < Val, std::is_base_of_v<MultiComponent, Val>>;
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
			ComponentCreator(Registry2& _registry, Entity _ent)
				: entity(_ent), m_registry(_registry)
			{}

			template<component_type Component, typename... Args>
			Component& add(Args&&... _args)
			{
				return m_registry.addComponent<Component>(entity, std::forward<Args>(_args)...);
			}

			const Entity entity;
		private:
			Registry2& m_registry;
		};
		template<typename Actor, typename... Args>
		Entity create(Args&&... _args)
		{
			Entity ent = create();
			Actor act(ComponentCreator(*this, ent), std::forward<Args>(_args)...);
			return ent;
		}

		// Remove an entity with all its components.
		void erase(Entity _ent)
		{
			ASSERT(m_generations[_ent.toIndex()].entity != INVALID_ENTITY, "Attempting to erase a non existent entity.");

			for (auto& components : m_components)
				if(components.contains(_ent.toIndex()))
					components.erase(_ent.toIndex());

			m_unusedEntities.push_back(_ent);
			m_generations[_ent.toIndex()].entity = INVALID_ENTITY;
		}

		// Add a new component to an existing entity. No changes are done if Component is
		// not a MultiComponent and _ent already has a component of this type.
		// @return A reference to the new component or the already existing component.
		template<component_type Component, typename... Args>
		Component& addComponent(Entity _ent, Args&&... _args)
		{
			return getContainer<Component>().template emplace<Component>(_ent.toIndex(), std::forward<Args>(_args)...);
		}

		template<component_type Component>
		void removeComponent(Entity _ent)
		{
			getContainer<Component>().erase(_ent.toIndex());
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
		Component& getComponentUnsafe(Entity _ent) { return getContainer<Component>().template at<Component>(_ent.toIndex()); }
		template<component_type Component>
		const Component& getComponentUnsafe(Entity _ent) const { getContainer<Component>().template at<Component>(_ent.toIndex()); }

		// Retrieve a component associated with an entity.
		template<component_type Component>
		Component* getComponent(Entity _ent) 
		{ 
			auto& container = getContainer<Component>();
			if (container.contains(_ent.toIndex()))
				return &container.template at<Component>(_ent.toIndex());
			else return nullptr;
		}
		template<component_type Component>
		const Component* getComponent(Entity _ent) const 
		{
			auto& container = getContainer<Component>();
			if (container.contains(_ent.toIndex()))
				return &container.template at<Component>(_ent.toIndex());
			else return nullptr;
		}

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

			friend class Registry2;

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
			auto mainContainer = getContainerUnsafe<Comp>().template iterate<Comp>();
			std::array< SM<Comp>*, sizeof...(Comps)> containers{ &getContainerUnsafe<Comps>()... };
		//	std::vector<SM<Comp>*> othContainers{ &getContainer<Comps>()... };

			for (auto it = mainContainer.begin(); it != mainContainer.end(); ++it)
			{
				Entity ent(it.key());
				auto hasComponents = [&]() 
				{
					for (SM<Comp>* comps : containers)
						if (!comps->contains(ent.toIndex())) return false;
					return true;
				};
				
				if(hasComponents())
		//		if ((getContainerUnsafe<Comps>().contains(ent.toIndex()) && ...))
				{
					executeHelper< WithEnt, 0, Comps...>(_action, ent, containers, it.value());
				/*	if constexpr (WithEnt)
						_action(_ent, it.value(), getContainerUnsafe<Comps>().at<Comps>(ent.toIndex())...);
					else
						_action(it.value(), getContainerUnsafe<Comps>().at<Comps>(ent.toIndex())...);*/
				}
			}
		}

		template<bool WithEntity, int Cur, component_type Comp, component_type... Comps, typename Action, typename... Args, size_t NumComps>
		void executeHelper(Action& _action, Entity _ent, std::array< SM<int>*, NumComps>& _containers, Args&... _args)
		{
			executeHelper<WithEntity, Cur+1, Comps...>(_action, _ent, _containers, _args..., _containers[Cur]->template at<Comp>(_ent.toIndex()));
		}

		template<bool WithEntity, int Cur, typename Action, typename... Args, size_t NumComps>
		void executeHelper(Action& _action, Entity _ent, std::array< SM<int>*, NumComps>& _containers,  Args&... _args)
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
		SM<Comp>& getContainer()
		{
			const int idx = m_typeIndex.value<Comp>();
			if (idx == m_components.size())
				m_components.emplace_back(static_cast<Comp*>(nullptr));
			return m_components[idx]; 
		}

		template<component_type Comp>
		SM<Comp>& getContainerUnsafe() { return m_components[m_typeIndex.value<Comp>()]; };

		template<component_type Comp>
		const SM<Comp>& getContainer() const { return m_components[m_typeIndex.value<Comp>()]; };

		std::vector<Entity> m_unusedEntities;
		uint32_t m_maxNumEntities = 0u;
		std::vector<SM<int>> m_components;
		std::vector<EntityRef> m_generations;
		utils::TypeIndex m_typeIndex;
	};
}
