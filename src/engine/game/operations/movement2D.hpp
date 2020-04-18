#pragma once

#include "../components/components2D.hpp"

namespace game {
namespace operations {

	class ApplyVelocity2D
	{
	public:
		ApplyVelocity2D(float _deltaTime) : m_deltaTime(_deltaTime) {}

		void operator()(const components::Velocity2D& _velocity, components::Position2D& _position) const;

	private:
		float m_deltaTime;
	};
}}