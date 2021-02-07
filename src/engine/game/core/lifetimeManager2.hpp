#pragma once

#include "registry2.hpp"
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

	// Type erased component vector.
	class WeakComponentVector
	{
	public:
		template<typename Comp>
		WeakComponentVector(utils::TypeHolder<Comp>, unsigned _initialCapacity = 4)
			: m_moveToRegistry(moveComponentsToRegistry<Comp>),
			m_clear(clear<Comp>),
			m_components(new char[_initialCapacity * sizeof(Comp)])
		{
			m_entities.reserve(_initialCapacity);
		}

		WeakComponentVector(WeakComponentVector&& _oth) noexcept;

		WeakComponentVector& operator=(WeakComponentVector&& _oth) noexcept;

		~WeakComponentVector()
		{
			m_clear(*this);
		}

		template<typename Value>
		const Value& get(size_t _pos) const { return *(reinterpret_cast<const Value*>(m_components.get()) + _pos); }

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
				for (size_t i = 0; i < prevCapacity; ++i)
				{
					new(&newBuf[i * sizeof(Component)]) Component(std::move(get<Component>(i)));
					get<Component>(i).~Component();
				}
				m_components.reset(newBuf);
			}

			Component& comp = *new(&get<Component>(m_entities.size()-1)) Component( std::forward<Args>(_args)... );
			return comp;
		}

		void clear()
		{
			m_clear(*this);
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

		template<typename Value>
		static void clear(WeakComponentVector& _container)
		{
			for (size_t i = 0; i < _container.m_entities.size(); ++i)
			{
				_container.get<Value>(i).~Value();
			}

			_container.m_entities.clear();
		}

		using MoveToRegistry = void(*)(Registry2&, WeakComponentVector&);
		MoveToRegistry m_moveToRegistry;
		using Clear = void(*)(WeakComponentVector&);
		Clear m_clear;

		std::vector<Entity> m_entities;
		std::unique_ptr<char[]> m_components;
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

		// override function explicitly so that it is not hidden by create<Actor>
		Entity create() { return EntityCreator::create(); }

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

		void cleanup()
		{
			EntityDeleter::cleanup(m_registry);
		}

		// Move newly created components into the registry.
		void moveComponents();
	private:

		template<component_type Comp>
		WeakComponentVector& getContainer()
		{
			auto it = m_newComponents.find(utils::TypeIndex::value<Comp>());
			if (it != m_newComponents.end())
				return it.data();

			m_newComponents.add(utils::TypeIndex::value<Comp>(),
				WeakComponentVector(utils::TypeHolder<Comp>()));
			return m_newComponents.find(utils::TypeIndex::value<Comp>()).data();
		}

		utils::HashMap<int, WeakComponentVector> m_newComponents;
	};

	using ComponentCreator = LifetimeManager2::ComponentCreator;
}