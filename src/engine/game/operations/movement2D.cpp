#include "movement2D.hpp"

namespace game { 
namespace operations {

	void ApplyVelocity2D::operator()(const components::Velocity2D& _velocity, components::Position2D& _position) const
	{
		_position.value += m_deltaTime * _velocity.value;
	}
}}
