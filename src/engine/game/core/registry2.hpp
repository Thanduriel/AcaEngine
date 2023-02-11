#pragma once

#include "../../utils/containers/hashmap.hpp"
#include "../../utils/metaProgHelpers.hpp"
#include "../../utils/typeIndex.hpp"
#include "../../utils/assert.hpp"
#include "entity.hpp"
#include "component.hpp"
#include "componentaccess.hpp"
#include <static_type_info.h>
#include <tuple>
#include <utility>
#include <type_traits>

namespace game {

	class Registry2
	{
		using TypeIndex = std::remove_const_t<static_type_info::TypeIndex>;
		template<typename T>
		using StorageMap = utils::HashMap<TypeIndex, T>;
	public:
		// Make a new entity managed by this registry.
		Entity create();

		// Remove an entity with all its components.
		void erase(Entity _ent);

		// Add a new component to an existing entity. No changes are done if Component is
		// _ent already has a component of this type.
		// @return A reference to the new component or the already existing component.
		template<component_type Component, typename... Args>
		requires (!std::same_as<Component, components::Parent>)
		Component& addComponent(Entity _ent, Args&&... _args)
		{
			return getContainer<Component>().template emplace<Component>(_ent.toIndex(), std::forward<Args>(_args)...);
		}

		template<component_type Component>
		requires (std::same_as<Component, components::Parent>)
		Component& addComponent(Entity _ent, components::Parent _parent)
		{
			auto& childs = addComponent<components::Children>(_parent.entity);
			childs.entities.push_back(_ent);
			return getContainer<components::Parent>().template emplace<Component>(_ent.toIndex(), _parent.entity);
		}

		// Remove a component from an entity.
		// Expects the component to exist.
		template<component_type Component>
		requires (!std::same_as<Component, components::Parent> && !std::same_as<Component, components::Children>)
		void removeComponent(Entity _ent)
		{
			getContainer<Component>().erase(_ent.toIndex());
		}

		// Also removes the entity from its parent list.
		// Unlike the base remove this checks for the existence of the component first.
		template<component_type Component>
		requires (std::same_as<Component, components::Parent>)
		void removeComponent(Entity _ent)
		{
			auto& comps = getContainer<Component>();
			if (comps.contains(_ent.toIndex()))
			{
				// remove from parent list
				auto& parent = comps.template at<Component>(_ent.toIndex());
				if (isValid(parent.entity))
				{
					auto& childs = getContainer<components::Children>().template at<components::Children>(parent.entity.toIndex()).entities;
					auto it = std::find(childs.begin(), childs.end(), _ent);
					ASSERT(it != childs.end(), "Child is not known to parent.");
					*it = childs.back();
					childs.pop_back();
				}
				comps.erase(_ent.toIndex());
			}
		}

		template<component_type Component>
		requires (std::same_as<Component, components::Children>)
		void removeComponent(Entity _ent)
		{
			auto& comps = getContainer<Component>();
			if (comps.contains(_ent.toIndex()))
			{
				auto& childs = comps.template at<Component>(_ent.toIndex()).entities;
				// erase all children
				for (Entity ent : childs)
					erase(ent);
				comps.erase(_ent.toIndex());
			}
		}

		// Remove all components of the specified type.
		// This is mostly useful for message and flag types.
		template<component_type Component>
		void clearComponent()
		{
			getContainer<Component>().clear();
		}

		template<data_component_type Component>
		bool hasComponent(Entity _ent) const { return getContainer<Component>()->contains(_ent.toIndex()); }

		// Retrieve a component associated with an entity.
		// Does not check whether it exits.
		template<data_component_type Component>
		Component& getComponentUnsafe(Entity _ent) { return getContainerUnsafe<Component>().template at<Component>(_ent.toIndex()); }
		template<data_component_type Component>
		const Component& getComponentUnsafe(Entity _ent) const { return getContainerUnsafe<Component>().template at<Component>(_ent.toIndex()); }

		// Retrieve a component associated with an entity.
		template<data_component_type Component>
		Component* getComponent(Entity _ent) 
		{ 
			auto& container = getContainer<Component>();
			if (container.contains(_ent.toIndex()))
				return &container.template at<Component>(_ent.toIndex());
			return nullptr;
		}
		template<data_component_type Component>
		const Component* getComponent(Entity _ent) const 
		{
			auto& container = getContainer<Component>();
			if (container && container->contains(_ent.toIndex()))
				return &container->template at<Component>(_ent.toIndex());
			return nullptr;
		}

		template<component_type Comp>
		ComponentStorage<Comp>& getContainer()
		{
			auto& map = std::get<StorageMap<ComponentStorage<Comp>>>(m_components);
			constexpr auto typeId = static_type_info::getTypeIndex<Comp>();
			auto it = map.find(typeId);
			
			return it != map.end() ? it.data() 
				: map.add(typeId, ComponentStorage<Comp>(utils::TypeHolder<Comp>{})).data();
		}

		// Execute an Action on all entities having the components
		// expected by Action::operator(...).
		template<typename Action>
		void execute(Action& _action) { executeUnpack(_action, utils::UnpackFunction(&Action::operator())); }
		// This explicit version is only needed to capture rvalues.
		template<typename Action>
		void execute(const Action& _action) { executeUnpack(_action, utils::UnpackFunction(&Action::operator())); }

		EntityRef getRef(Entity _ent) const { return m_generations[_ent.toIndex()]; }

		// Checks whether an entity is managed by this registry.
		// An Entity can be valid even if previously deleted, if the id was reassigned.
		// Use an EntityRef instead to prevent this possibility.
		bool isValid(Entity _ent) const { return _ent && m_generations[_ent.toIndex()].entity == _ent; }

		// Attempt to retrieve the referenced entity.
		// @return The entity or an INVALID_ENTITY if the ref is not valid.
		Entity getEntity(EntityRef _ent) const
		{
			if (_ent.entity.toIndex() > static_cast<Entity::BaseType>(m_generations.size()))
				return INVALID_ENTITY;
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
				executeImpl<true, Action, std::decay_t<Comps>...>(_action, std::make_index_sequence<sizeof...(Comps)-1>{});
			else
				executeImpl<false, Action, std::decay_t<Comp>, std::decay_t<Comps>...>(_action, std::make_index_sequence<sizeof...(Comps)>{});
		}

		template<bool WithEnt, typename Action, component_type Comp, component_type... Comps, std::size_t... I>
		void executeImpl(Action& _action, std::index_sequence<I...>)
		{
			auto mainContainer = getContainer<Comp>().template iterate<Comp>();
			auto othContainers = std::tie(getContainer<Comps>()...);
		
			for (auto it = mainContainer.begin(); it != mainContainer.end(); ++it)
			{
				Entity ent(it.key());
				
				if ((std::get<I>(othContainers).contains(ent.toIndex()) && ...))
				{
					if constexpr (WithEnt)
						_action(ent, it.value(), std::get<I>(othContainers).template at<Comps>(ent.toIndex()) ...);
					else
						_action(it.value(), std::get<I>(othContainers).template at<Comps>(ent.toIndex()) ...);
				}
			}
		}

		// returns a pointer because the container might not exist yet
		template<component_type Comp>
		const ComponentStorage<Comp>* getContainer() const
		{
			auto& map = std::get<StorageMap<ComponentStorage<Comp>>>(m_components);
			auto it = map.find(static_type_info::getTypeIndex<Comp>());
			
			return it != map.end() ? &it.data() : nullptr;
		}

		template<component_type Comp>
		ComponentStorage<Comp>& getContainerUnsafe() 
		{
			auto& map = std::get<StorageMap<ComponentStorage<Comp>>>(m_components);
			return map.find(static_type_info::getTypeIndex<Comp>()).data(); 
		}

		template<component_type Comp>
		const ComponentStorage<Comp>& getContainerUnsafe() const 
		{
			auto& map = std::get<StorageMap<ComponentStorage<Comp>>>(m_components);
			return map.find(static_type_info::getTypeIndex<Comp>()).data(); 
		}

		std::vector<Entity> m_unusedEntities;
		Entity::BaseType m_maxNumEntities = 0u;
		std::tuple<StorageMap<DataStorage>,
			StorageMap<MessageStorage>,
			StorageMap<FlagStorage>> m_components;
		std::vector<EntityRef> m_generations;
	};
}
