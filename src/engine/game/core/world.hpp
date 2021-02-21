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

		template<typename System>
		void run(System& _system) { executeUnpack(_system, utils::UnpackFunction(&System::update)); }
		// This explicit version is only needed to capture rvalues.
		template<typename System>
		void run(const System& _system) { executeUnpack(_system, utils::UnpackFunction(&System::update)); }

	private:
		template<typename System, typename... Resources>
		void executeUnpack(System& _system, utils::UnpackFunction<std::remove_cv_t<System>, Resources...>)
		{
			_system.update(ResourceFetch<Resources>::get(*this)...);
		}

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

		float m_deltaTime;
		Registry2 m_registry;
	};
}