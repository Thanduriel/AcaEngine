#include "applyVelocity.hpp"

namespace game {
namespace actions {

	void ApplyVelocity::operator()(const components::Velocity& _velocity, components::Position& _position) const
	{
		_position.value += m_deltaTime * _velocity.value;
	}
}}
