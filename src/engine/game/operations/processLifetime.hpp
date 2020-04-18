#pragma once

#include "../core/entity.hpp"
#include "../core/lifetimeManager.hpp"
#include "../components/simpleComponents.hpp"
#include "../../graphics/renderer/meshrenderer.hpp"

namespace game {
namespace operations {

	class ProcessLifetime
	{
	public:
		ProcessLifetime(LifetimeManager& _manager, float _deltaTime) : m_manager(_manager), m_deltaTime(_deltaTime) {}
		void operator()(Entity _ent, components::Lifetime& _lifeTime) const;

	private:
		LifetimeManager& m_manager;
		float m_deltaTime;
	};
}}
