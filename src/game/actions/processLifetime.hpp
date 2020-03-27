#pragma once

#include "game/core/entity.hpp"
#include "game/core/action.hpp"
#include "game/core/lifetimeManager.hpp"
#include "game/components/simpleComponents.hpp"
#include "graphics/renderer/meshrenderer.hpp"

namespace game {
namespace actions {

	class ProcessLifetimeImpl
	{
	public:
		ProcessLifetimeImpl(LifetimeManager& _manager, float _deltaTime) : m_manager(_manager), m_deltaTime(_deltaTime) {}
		void operator()(Entity _ent, Lifetime& _lifeTime);

	private:
		LifetimeManager& m_manager;
		float m_deltaTime;
	};

	using ProcessLifetime = Action<ProcessLifetimeImpl, Lifetime>;
}}