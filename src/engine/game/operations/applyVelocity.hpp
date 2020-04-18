#pragma once

#include "../components/simpleComponents.hpp"

namespace game {
namespace operations {

	class ApplyVelocity
	{
	public:
		ApplyVelocity(float _deltaTime) : m_deltaTime(_deltaTime) {}

		void operator()(const components::Velocity& _velocity, components::Position& _position) const;

	private:
		float m_deltaTime;
	};
}}