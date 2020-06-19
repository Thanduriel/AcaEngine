#pragma once

#include "../core/entity.hpp"
#include "../core/lifetimeManager.hpp"
#include "../components/simpleComponents.hpp"
#include "../../graphics/renderer/meshrenderer.hpp"

namespace game {
namespace operations {

	template<typename Manager>
	class ProcessLifetime
	{
	public:
		ProcessLifetime(Manager& _manager, float _deltaTime) : m_manager(_manager), m_deltaTime(_deltaTime) {}
		void operator()(Entity _ent, components::Lifetime& _lifeTime) const
		{
			_lifeTime.timeLeft -= m_deltaTime;
			if (_lifeTime.timeLeft <= 0.f) m_manager.destroy(_ent);
		}
	private:
		Manager& m_manager;
		float m_deltaTime;
	};
}}
