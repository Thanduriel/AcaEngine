#include "processLifetime.hpp"

namespace game {
namespace actions {

	void ProcessLifetime::operator()(Entity _ent, Lifetime& _lifeTime) const
	{
		_lifeTime.timeLeft -= m_deltaTime;
		if (_lifeTime.timeLeft <= 0.f) m_manager.destroy(_ent);
	}

}}
