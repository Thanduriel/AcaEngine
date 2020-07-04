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


	using namespace components;

	Hud::Hud()
		: m_this(m_registry.create()),
		m_camera(graphics::Device::getBufferSize(), glm::vec2(0.f))
	{
		m_registry.addComponent<Position2D>(m_this, glm::vec2(0.f));
		m_registry.addComponent<BoundingRectangle>(m_this, graphics::Device::getBufferSize(), glm::vec2(0.f));
	}

	void Hud::draw()
	{
		m_registry.execute(operations::DrawSprites2D(m_spriteRenderer));
		m_spriteRenderer.present(m_camera);
		m_spriteRenderer.clear();
	}

	glm::vec2 Hud::getAbsolutePosition(Entity _entity, glm::vec2 _relativePosition) const
	{
		const auto& pos = m_registry.getComponent<Position2D>(_entity);
		const auto& rect = m_registry.getComponent<BoundingRectangle>(_entity);

		const glm::vec2 min = pos.value - rect.center * rect.size;
		return min + _relativePosition * rect.size;
	}
}