#include "applyVelocity.hpp"

namespace game { 
namespace operations {

	void ApplyVelocity::operator()(const components::Velocity& _velocity, components::Position& _position) const
	{
		_position.value += m_deltaTime * _velocity.value;
	}

	void ApplyAngularVelocity::operator()(const components::AngularVelocity& _velocity, components::Rotation& _rot) const {
		// FIXME: not valid for dt > 1
		const glm::quat& vel = _velocity.value;
		_rot.value *= glm::slerp(vel,vel*vel, m_deltaTime);
	}
}}
