#include "hud.hpp"
#include "../operations/drawSprites.hpp"
#include "../../graphics/core/device.hpp"

namespace game {

	struct UpdateFillBar
	{
		void operator()(components::Sprite& _sprite, 
			const components::Position2D& _position, 
			const components::Rotation2D& _rotation)
		{}
	};

	Hud::Hud()
		: m_camera(graphics::Device::getBufferSize(), glm::vec2(0.f))
	{}

	void Hud::draw()
	{
		m_registry.execute(operations::DrawSprites2D(m_spriteRenderer));
		m_spriteRenderer.present(m_camera);
		m_spriteRenderer.clear();
	}
}