#pragma once

#include "registry2.hpp"
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

	// Type erased component vector.
	class WeakComponentVector
	{
	public:
		template<typename Comp>
		WeakComponentVector(Comp* comp, unsigned _initialCapacity = 4)
			: m_moveToRegistry(moveComponentsToRegistry<Comp>),
			m_components(new char[_initialCapacity * sizeof(Comp)])
		{
			m_entities.reserve(_initialCapacity);
		}

		template<typename Value>
		const Value& get(size_t _pos) const { return *(reinterpret_cast<Value*>(m_components.get()) + _pos); }

		template<typename Value>
		Value& get(size_t _pos) { return *(reinterpret_cast<Value*>(m_components.get()) + _pos); }

		template<component_type Component, typename... Args>
		Component& emplace(Entity _ent, Args&&... _args)
		{
			const size_t prevCapacity = m_entities.capacity();
			m_entities.push_back(_ent);
			const size_t capacity = m_entities.capacity();
			if (prevCapacity != capacity)
			{
				char* newBuf = new char[m_entities.capacity() * sizeof(Component)];
				for (size_t i = 0; i < capacity; ++i)
				{
					new(&newBuf[i * sizeof(Component)]) Component(std::move(get<Component>(i)));
					get<Component>(i).~Component();
				}
				m_components.reset(newBuf);
			}

			Component& comp = *new(&get<Component>(m_entities.size()-1)) Component{ std::forward<Args>(_args)... };
			return comp;
		}

		void moveToRegistry(Registry2& _registry)
		{
			m_moveToRegistry(_registry, *this);
		}

	private:
		template<typename Value>
		static void moveComponentsToRegistry(Registry2& registry, WeakComponentVector& _container)
		{
			for (size_t i = 0; i < _container.m_entities.size(); ++i)
			{
				Value& comp = _container.get<Value>(i);
				registry.addComponent<Value>(_container.m_entities[i], std::move(comp));
				comp.~Value();
			}

			_container.m_entities.clear();
		}

		std::vector<Entity> m_entities;
		std::unique_ptr<char[]> m_components;

		using MoveToRegistry = void(*)(Registry2&, WeakComponentVector&);
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
			WeakComponentVector& container = getContainer<Component>();

			return container.emplace<Component>(_ent, std::forward<Args>(_args)...);
		}

		class ComponentCreator
		{
		public:
			template<component_type Component, typename... Args>
			Component& add(Args&&... _args) const
			{
				return m_manager.addComponent<Component>(entity, std::forward<Args>(_args)...);
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

		// Move newly created components into the registry.
		void moveComponents();
	private:

		template<component_type Comp>
		WeakComponentVector& getContainer()
		{
			const int idx = m_typeIndex.value<Comp>();
			if (idx == m_newComponents.size())
			{
				m_newComponents.emplace_back(static_cast<Comp*>(nullptr));
			}
			return m_newComponents[idx];
		}

		std::vector<WeakComponentVector> m_newComponents;
		utils::TypeIndex m_typeIndex;
	};

	using ComponentCreator = LifetimeManager2::ComponentCreator;
}