#include "processLifetime.hpp"

namespace game {
namespace operations {

	void ProcessLifetime::operator()(Entity _ent, components::Lifetime& _lifeTime) const
	{
		_lifeTime.timeLeft -= m_deltaTime;
		if (_lifeTime.timeLeft <= 0.f) m_manager.destroy(_ent);
	}

}}
