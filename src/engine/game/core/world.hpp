#pragma once

#include "registry2.hpp"
#include "lifetimeManager2.hpp"
#include <static_type_info.h>

namespace game {

	template<typename T>
	concept system_type = requires { &T::update; };

	enum struct SystemGroup 
	{
		Process,
		Draw,
		COUNT
	};

	template<typename... Resources>
	class World
	{
	public:
		World(Resources&&... _resources) 
			: m_resources(EntityCreator(m_registry), std::forward<Resources>(_resources)...),
			m_deltaTime(0.f),
			m_manager(m_registry)
		{}

		void process(SystemGroup _group, float _deltaTime)
		{
			m_deltaTime = _deltaTime;
			auto& systemGroup = m_systems[static_cast<size_t>(_group)];
			for (auto& sysHolder : systemGroup)
				sysHolder.runFn(*this, sysHolder.system.get());
		}

		void cleanup()
		{
			m_manager.moveComponents();
			m_manager.cleanup();
			m_deleter.cleanup(m_registry);
		}

		// direct resource access; should not be used
		Registry2& getRegistry() { return m_registry; }
		// only for debugging
		LifetimeManager2& getManager() { return m_manager; }
		// currently only for static resources
		template<typename Resource>
		Resource& getResource() { return std::get<Resource>(m_resources); }

		// Create a dynamic resource explicitly.
		// If a resource is default constructible it will also be initialized
		// automatically when a system requesting it is registered.
		// @param _args Arguments forwarded to the constructor of a Resource.
		template<typename Resource, typename... Args>
		Resource& addResource(Args&&... _args)
		{
			Resource* r = new Resource(std::forward<Args>(_args)...);
			m_dynamicResources.add(static_type_info::getTypeIndex<Resource>(),
				UniquePtr(r,&destroyObject<Resource>));
			return *r;
		}

		template<system_type System, system_type... Dependencies>
		System* addSystem(std::unique_ptr<System> _system, SystemGroup _group, 
			const Dependencies*... _dependencies)
		{
			auto& systemGroup = m_systems[static_cast<size_t>(_group)];
			System* system = _system.release();
			systemGroup.emplace_back(UniquePtr(system, &destroyObject<System>),
				&runSystem<System>,
				std::vector<const void*>{static_cast<const void*>(_dependencies)...});
			scanRequirements(*system, utils::UnpackFunction(&System::update));
			return system;
		}

		// Call member function of a system providing the expected resources.
		template<typename System, typename... ResourcesReq>
		void call(System& _system, void(System::* _func)(ResourcesReq ...)) { (_system.*_func)(ResourceFetch<ResourcesReq>::get(*this)...); }
		template<typename System, typename... ResourcesReq>
		void call(const System& _system, void(System::* _func)(ResourcesReq ...) const) { (_system.*_func)(ResourceFetch<ResourcesReq>::get(*this)...); }

		template<typename System>
		void run(System& _system) { call(_system, &System::update); }
		// This explicit version is only needed to capture rvalues.
		template<typename System>
		void run(const System& _system) { call(_system, &System::update); }
	private:
		// Default construct a required resource if it does not exist yet.
		template<typename Res>
		void ensureResource()
		{
			using ResourceT = std::remove_cvref_t<Res>;
			// check that it is a dynamic resource that can be default constructed
			if constexpr (!std::is_default_constructible_v<ResourceFetch<Res>>
				&& !utils::contains_type_v<ResourceT, decltype(m_resources)>
				&& std::is_default_constructible_v<ResourceT>)
			{
				constexpr auto typeId = static_type_info::getTypeIndex<ResourceT>();
				auto it = m_dynamicResources.find(typeId);
				if (it == m_dynamicResources.end())
					addResource<ResourceT>();
			}
		}

		template<typename System, typename... ResourcesReq>
		void scanRequirements(System& _system, utils::UnpackFunction<System,ResourcesReq...>)
		{
			(ensureResource<ResourcesReq>(), ...);
		}

		// Generic lookup in the resource tuple.
		// Handles both const and mutable reference types 
		// since the unqualified reference is returned.
		template<typename Resource>
		struct ResourceFetch
		{
			ResourceFetch() = delete; // mark for ensureResource

			static Resource& get(World& world)
			{
				using ResourceType = std::remove_cvref_t<Resource>;
				if constexpr(utils::contains_type_v<ResourceType, decltype(world.m_resources)>)
					return std::get<ResourceType>(world.m_resources);
				else
				{
					void* ptr = world.m_dynamicResources.find(static_type_info::getTypeIndex<ResourceType>()).data().get();
					return *reinterpret_cast<ResourceType*>(ptr);
				}
			}
		};

		// game time
		template<>
		struct ResourceFetch<float>
		{
			static float get(World& world)
			{
				return world.m_deltaTime;
			}
		};

		template<>
		struct ResourceFetch<EntityDeleter&>
		{
			static EntityDeleter& get(World& world)
			{
				return world.m_deleter;
			}
		};

		template<>
		struct ResourceFetch<LifetimeManager2&>
		{
			static LifetimeManager2& get(World& world)
			{
				return world.m_manager;
			}
		};

		// Read component
		template<typename Comp>
		struct ResourceFetch <ReadAccess<Comp>>
		{
			static ReadAccess<Comp> get(World& world)
			{
				return ReadAccess<Comp>(world.m_registry.getContainer<Comp>());
			}
		};

		// Write component
		template<typename Comp>
		struct ResourceFetch <WriteAccess<Comp>>
		{
			static WriteAccess<Comp> get(World& world)
			{
				return WriteAccess<Comp>(world.m_registry.getContainer<Comp>());
			}
		};

		// Component Tuple
		template<typename... CompAccess>
		struct ResourceFetch <ComponentTuple<CompAccess...>>
		{
			static ComponentTuple<CompAccess...> get(World& world)
			{
				return ComponentTuple<CompAccess...>(ResourceFetch<CompAccess>::get(world)...);
			}
		};

		using RunSystem = void(*)(World&, void*);
		template<typename System>
		static void runSystem(World& _world, void* _sys)
		{
			System& sys = *reinterpret_cast<System*>(_sys);
			_world.call(sys, &System::update);
		}

		using DestroyObject = void(*)(void*);
		template<typename T>
		static void destroyObject(void* _obj)
		{
			T* obj = reinterpret_cast<T*>(_obj);
			delete obj;
		}

		float m_deltaTime;
		Registry2 m_registry;
		EntityDeleter m_deleter;
		LifetimeManager2 m_manager; // legacy all in one interface

		std::tuple<EntityCreator, Resources...> m_resources;
		using TypeIndex = std::remove_const_t<static_type_info::TypeIndex>;
		using UniquePtr = std::unique_ptr<void, DestroyObject>;
		utils::HashMap<TypeIndex, UniquePtr> m_dynamicResources;
		
		struct SystemHolder
		{
			UniquePtr system;
			RunSystem runFn;
			std::vector<const void*> dependencies;
		};

		std::array< std::vector<SystemHolder>, static_cast<size_t>(SystemGroup::COUNT)> m_systems;
	};
}