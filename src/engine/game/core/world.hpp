#pragma once

#include "registry2.hpp"

namespace game {

	class World
	{
	public:
		void process(float _deltaTime)
		{
			m_deltaTime = _deltaTime;
		}

		Registry2& getRegistry() { return m_registry; }

		// Call member function of a system providing the expected resources.
		template<typename System, typename... Resources>
		void call(System& _system, void(System::* _func)(Resources ...)) { (_system.*_func)(ResourceFetch<Resources>::get(*this)...); }
		template<typename System, typename... Resources>
		void call(const System& _system, void(System::* _func)(Resources ...) const) { (_system.*_func)(ResourceFetch<Resources>::get(*this)...); }

		template<typename System>
		void run(System& _system) { call(_system, &System::update); }
		// This explicit version is only needed to capture rvalues.
		template<typename System>
		void run(const System& _system) { call(_system, &System::update); }
	private:
		template<typename Resource>
		struct ResourceFetch;

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
	};
}