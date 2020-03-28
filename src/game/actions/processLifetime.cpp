#include "processLifetime.hpp"

namespace game {
namespace actions {

	void ProcessLifetimeImpl::operator()(Entity _ent, Lifetime& _lifeTime) const
	{
		_lifeTime.timeLeft -= m_deltaTime;
		if (_lifeTime.timeLeft <= 0.f) const_cast<LifetimeManager&>(m_manager).destroy(_ent);
	}

}}
