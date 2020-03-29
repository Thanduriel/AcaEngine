#pragma once

#include "game/core/action.hpp"
#include "game/components/simpleComponents.hpp"

namespace game {
namespace actions {

	class ApplyVelocity
	{
	public:
		ApplyVelocity(float _deltaTime) : m_deltaTime(_deltaTime) {}

		void operator()(const Velocity& _velocity, Position& _position) const;

	private:
		float m_deltaTime;
	};
}}