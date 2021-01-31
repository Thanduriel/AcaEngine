#pragma once

#include "registry2.hpp"
#include <vector>

namespace game {

	class WeakComponentVector
	{
	public:
		template<typename Comp>
		WeakComponentVector(Comp* comp)
			: m_moveToRegistry(moveComponentsToRegistry<Comp>)
		{
			m_entities.reserve(4);
			m_components = new char[m_entities.capacity() * sizeof(Comp)];
		}

		template<typename Value>
		const Value& get(size_t _pos) const { return *(reinterpret_cast<Value*>(m_components) + _pos); }

		template<typename Value>
		Value& get(size_t _pos) { return *(reinterpret_cast<Value*>(m_components) + _pos); }

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

			Component& comp = *new(&get<Component>()) Component{ std::forward<Args>(_args)... };
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
				registry.addComponent(_container.m_entities[i], std::move(comp));
				comp.~Value();
			}

			_container.m_entities.clear();
		}

		std::vector<Entity> m_entities;
		std::unique_ptr<char[]> m_components;

		using MoveToRegistry = void(*)(Registry2&, WeakComponentVector&);
		MoveToRegistry m_moveToRegistry;
	};

	// Handles ordered construction and deletion of entities.
	class LifetimeManager2
	{
	public:
		LifetimeManager2(Registry2& _registry) : m_registry(_registry) {}

		Entity create();

		void destroy(Entity _ent);

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
		void moveComponents()
		{
			for (WeakComponentVector& container : m_newComponents)
				container.moveToRegistry(m_registry);
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

		Registry2& m_registry;
		std::vector<Entity> m_deleted;
		std::vector<WeakComponentVector> m_newComponents;

		utils::TypeIndex m_typeIndex;
	};
}