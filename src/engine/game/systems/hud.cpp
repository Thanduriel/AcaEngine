#include "hud.hpp"

namespace game {

	struct UpdateFillBar
	{
		void operator()(components::Sprite& _sprite, 
			const components::Position2D& _position, 
			const components::Rotation2D& _rotation)
		{}
	};

	Hud::Hud()
		: m_camera(glm::vec2(1.f,1.f))
	{}
}