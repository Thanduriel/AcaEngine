#pragma once

#include "registry2.hpp"
#include "weakcomponentvector.hpp"
#include "../../utils/containers/hashmap.hpp"
#include <vector>

namespace game {

	class EntityCreator
	{
	public:
		EntityCreator(Registry2& _registry);

		Entity create();

	protected:
		Registry2& m_registry;
	};

	// Resource to savely delete entities.
	class EntityDeleter
	{
	public:
		// Marks entity for deletion.
		void destroy(Entity _ent);

		// Remove marked entities.
		void cleanup(Registry2& _registry);

	private:
		std::vector<Entity> m_deleted;
	};

	class TempComponentVector : public WeakComponentVector<Entity::BaseType>
	{
	public:
		template<component_type Component>
		TempComponentVector(utils::TypeHolder<Component> _type)
			: WeakComponentVector(_type),
			m_moveToRegistry(moveComponentsToRegistry<Component>)
		{}

		void moveToRegistry(Registry2& _registry)
		{
			m_moveToRegistry(_registry, *this);
		}

	private:
		template<typename Value>
		static void moveComponentsToRegistry(Registry2& registry, TempComponentVector& _container)
		{
			for(auto [entity, comp] : _container.iterate<Value>())
			{
				registry.addComponent<Value>(Entity(entity), std::move(comp));
			}

			_container.clear();
		}

		using MoveToRegistry = void(*)(Registry2&, TempComponentVector&);
		MoveToRegistry m_moveToRegistry;
	};

	// Handles orderly construction and deletion of entities and components.
	class LifetimeManager2 : public EntityDeleter, public EntityCreator
	{
	public:
		LifetimeManager2(Registry2& _registry) : EntityCreator(_registry) {}

		template<component_type Component, typename... Args>
		Component& addComponent(Entity _ent, Args&&... _args)
		{
			TempComponentVector& container = getContainer<Component>();

			return container.emplace<Component>(_ent.toIndex(), std::forward<Args>(_args)...);
		}

		// override function explicitly so that it is not hidden by create<Actor>
		Entity create() { return EntityCreator::create(); }

		class ComponentCreator
		{
		public:
			// returns a reference to the newly created component
			template<component_type Component, typename... Args>
			Component& addR(Args&&... _args) const
			{
				return m_manager.addComponent<Component>(entity, std::forward<Args>(_args)...);
			}

			// returns the creator to chain add expressions together
			template<component_type Component, typename... Args>
			const ComponentCreator& add(Args&&... _args) const
			{
				m_manager.addComponent<Component>(entity, std::forward<Args>(_args)...);
				return *this;
			}

			ComponentCreator child() const
			{
				auto creator = ComponentCreator(m_manager, m_manager.create());
				creator.add<components::Parent>(entity);
				return creator;
			}

			const Entity entity;

		private:
			friend class LifetimeManager2;
			ComponentCreator(LifetimeManager2& _manager, Entity _ent)
				: entity(_ent), m_manager(_manager)
			{}

			LifetimeManager2& m_manager;
		};

		template<typename Actor, typename... Args>
		Entity create(Args&&... _args)
		{
			Entity ent = create();
			ComponentCreator creator(*this, ent);
			Actor act(creator, std::forward<Args>(_args)...);
			return ent;
		}

		void cleanup()
		{
			EntityDeleter::cleanup(m_registry);
		}

		// Move newly created components into the registry.
		void moveComponents();
	private:

		template<component_type Comp>
		TempComponentVector& getContainer()
		{
			auto it = m_newComponents.find(utils::TypeIndex::value<Comp>());
			if (it != m_newComponents.end())
				return it.data();

			return m_newComponents.add(utils::TypeIndex::value<Comp>(),
				TempComponentVector(utils::TypeHolder<Comp>())).data();
		}

		utils::HashMap<int, TempComponentVector> m_newComponents;
	};

	using ComponentCreator = LifetimeManager2::ComponentCreator;
}