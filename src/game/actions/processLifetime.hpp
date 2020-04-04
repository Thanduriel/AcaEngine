#pragma once

#include "game/core/entity.hpp"
#include "game/core/action.hpp"
#include "game/core/lifetimeManager.hpp"
#include "game/components/simpleComponents.hpp"
#include "graphics/renderer/meshrenderer.hpp"

namespace game {
namespace actions {

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
