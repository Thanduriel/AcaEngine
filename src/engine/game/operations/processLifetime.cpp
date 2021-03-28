#include "processLifetime.hpp"

namespace game {
namespace systems {
	void ProcessLifetime::update(float _deltaTime, 
		EntityDeleter& _deleter, 
		Components _comps) const
	{
		_comps.execute([&](Entity ent, components::Lifetime& lifeTime)
			{
				lifeTime.timeLeft -= _deltaTime;
				if (lifeTime.timeLeft <= 0.f) _deleter.destroy(ent);
			});
	}
}}
