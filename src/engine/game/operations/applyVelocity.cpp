#include "applyVelocity.hpp"

namespace game { 
namespace operations {

	void ApplyVelocity::operator()(const components::Velocity& _velocity, components::Position& _position) const
	{
		_position.value += m_deltaTime * _velocity.value;
	}

	void ApplyAngularVelocity::operator()(const components::AngularVelocity& _velocity
		, components::Rotation& _rotation) const
	{
		_rotation.value = glm::angleAxis(_velocity.speed * m_deltaTime, _velocity.axis) 
			* _rotation.value;
	}
}}
