#include "applyVelocity.hpp"

namespace game {
namespace actions {

	void ApplyVelocityImpl::operator()(const Velocity& _velocity, Position& _position) const
	{
		_position += m_deltaTime * _velocity;
	}
}}