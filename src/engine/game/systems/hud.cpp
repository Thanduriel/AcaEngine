#include "hud.hpp"
#include "../operations/drawSprites.hpp"
#include "../../graphics/core/device.hpp"
#include <engine/input/inputmanager.hpp>

namespace game {

	struct UpdateFillBar
	{
		void operator()(components::Sprite& _sprite, 
			const components::Position2D& _position, 
			const components::Rotation2D& _rotation)
		{}
	};

	struct ClickButton
	{
		ClickButton(glm::vec2 _cursorPos) : cursorPos(_cursorPos) {}

		glm::vec2 cursorPos;

		void operator()(const components::Button& _button,
			const components::BoundingRectangle& _bounds,
			const components::Position2D& _position) const
		{
			const glm::vec2 min = _position.value - _bounds.center * _bounds.size;
			const glm::vec2 max = min + _bounds.size;
			if (cursorPos.x >= min.x && cursorPos.x <= max.x && cursorPos.y >= min.y && cursorPos.y <= max.y)
				_button.onClick();
		}
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

	void Hud::processInputs()
	{
		if (input::InputManager::isButtonPressed(input::MouseButton::LEFT))
		{
			glm::vec2 cursorPos = input::InputManager::getCursorPos();
			cursorPos.y = m_registry.getComponent<BoundingRectangle>(m_this).size.y - cursorPos.y;
			m_registry.execute(ClickButton(cursorPos));
		}
	}

	glm::vec2 Hud::getAbsolutePosition(Entity _entity, glm::vec2 _relativePosition) const
	{
		const auto& pos = m_registry.getComponent<Position2D>(_entity);
		const auto& rect = m_registry.getComponent<BoundingRectangle>(_entity);

		const glm::vec2 min = pos.value - rect.center * rect.size;
		return min + _relativePosition * rect.size;
	}
}