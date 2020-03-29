#include "applyVelocity.hpp"

namespace game {
namespace actions {

	void ApplyVelocity::operator()(const Velocity& _velocity, Position& _position) const
	{
		_position += m_deltaTime * _velocity;
	}
}}