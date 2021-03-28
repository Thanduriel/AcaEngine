#pragma once

#include "registry2.hpp"
#include "lifetimeManager2.hpp"

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
			: m_resources(std::forward<Resources>(_resources)...),
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
		template<typename Resource>
		Resource& getResource() { return std::get<Resource>(m_resources); }

		template<system_type System, system_type... Dependencies>
		System* addSystem(std::unique_ptr<System> _system, SystemGroup _group, 
			const Dependencies*... _dependencies)
		{
			auto& systemGroup = m_systems[static_cast<size_t>(_group)];
			System* system = _system.release();
			systemGroup.emplace_back(std::unique_ptr<void, DestroySystem>(system, &destroySystem<System>),
				&runSystem<System>,
				std::vector<const void*>{static_cast<const void*>(_dependencies)...});
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
		// generic lookup in the resource tuple
		template<typename Resource>
		struct ResourceFetch
		{
			static Resource& get(World& world)
			{
				return std::get<std::remove_cvref_t<Resource>>(world.m_resources);
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
		using DestroySystem = void(*)(void*);
		template<typename System>
		static void destroySystem(void* _sys)
		{
			System* sys = reinterpret_cast<System*>(_sys);
			delete sys;
		}

		struct SystemHolder
		{
			std::unique_ptr<void, DestroySystem> system;
			RunSystem runFn;
			std::vector<const void*> dependencies;
		};

		float m_deltaTime;
		Registry2 m_registry;
		EntityDeleter m_deleter;
		LifetimeManager2 m_manager; // legacy all in one interface
		std::tuple<Resources...> m_resources;
		std::array< std::vector<SystemHolder>, static_cast<size_t>(SystemGroup::COUNT)> m_systems;
	};
}