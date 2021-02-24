#pragma once

#include "registry2.hpp"

namespace game {

	template<typename... Resources>
	class World
	{
	public:
		World(Resources&&... _resources) 
			: m_resources(std::forward<Resources>(_resources)...),
			m_deltaTime(0.f) {}

		void process(float _deltaTime)
		{
			m_deltaTime = _deltaTime;
		}

		// direct resource access; should not be used
		Registry2& getRegistry() { return m_registry; }
		template<typename Resource>
		Resource& getResource() { return std::get<Resource>(m_resources); }

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

		float m_deltaTime;
		Registry2 m_registry;
		std::tuple<Resources...> m_resources;
	};
}