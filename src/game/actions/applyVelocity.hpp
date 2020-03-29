#pragma once

#include "game/core/action.hpp"
#include "game/components/simpleComponents.hpp"

namespace game {
namespace actions {

	class ApplyVelocityImpl
	{
	public:
		ApplyVelocityImpl(float _deltaTime) : m_deltaTime(_deltaTime) {}

		void operator()(const Velocity& _velocity, Position& _position) const;

	private:
		float m_deltaTime;
	};

	using ApplyVelocity = ApplyVelocityImpl;//Action<ApplyVelocityImpl, Velocity, Position>;
}}