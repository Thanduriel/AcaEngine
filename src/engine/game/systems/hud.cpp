#include "hud.hpp"
#include "../operations/drawSprites.hpp"
#include "../operations/drawText.hpp"
#include "../../graphics/core/device.hpp"
#include "../../graphics/renderer/fontrenderer.hpp"
#include "../../input/inputmanager.hpp"

namespace game {

	using namespace components;

	struct ClickButton
	{
		ClickButton(glm::vec2 _cursorPos) : cursorPos(_cursorPos) {}

		glm::vec2 cursorPos;

		void operator()(const Button& _button,
			const BoundingRectangle& _bounds,
			const Position2D& _position) const
		{
			const glm::vec2 min = _position.value - _bounds.alignment * _bounds.size;
			const glm::vec2 max = min + _bounds.size;
			if (cursorPos.x >= min.x && cursorPos.x <= max.x && cursorPos.y >= min.y && cursorPos.y <= max.y)
				_button.onClick();
		}
	};

	Hud::Hud(graphics::FontRenderer* _fontRenderer)
		: m_manager(m_registry),
		m_this(m_registry.create()),
		m_fontRenderer(_fontRenderer),
		m_camera(graphics::Device::getBufferSize(), glm::vec2(0.f))
	{
		m_registry.addComponent<Position2D>(m_this, glm::vec2(0.f));
		m_registry.addComponent<BoundingRectangle>(m_this, graphics::Device::getBufferSize(), glm::vec2(0.f));
	}

	void Hud::process()
	{
		using namespace glm;

		if (m_fontRenderer)
		{
			auto updateLabels = [this](BoundingRectangleNeedsUpdate, Label& label, BoundingRectangle& box)
			{
				box = BoundingRectangle(m_fontRenderer->getBoundingBox(
					vec3(0.f),
					label.text.c_str(),
					label.fontSize,
					label.alignment.x,
					label.alignment.y,
					label.roundToPixel));
			};
			m_registry.execute(updateLabels);
		}

		auto updateAutoContainers = [this](BoundingRectangleNeedsUpdate,
			AutoArrange& autoArrange, 
			BoundingRectangle& box,
			const Transform2D& transform,
			Children& children)
		{
			vec2 currentPos = transform.position;
			vec2 currentBounds = {};
			for (Entity child : children.entities)
			{
				const BoundingRectangle& bounds = m_registry.getComponentUnsafe<BoundingRectangle>(child);
				Transform2D& childTransform = m_registry.getComponentUnsafe<Transform2D>(child);
				childTransform.position = currentPos + vec2(0.f, currentBounds.y) + bounds.alignment * bounds.size;
				currentBounds.x = std::max(currentBounds.x, bounds.size.x);
				currentBounds.y -= bounds.size.y;
			}
			box.size = currentBounds;
		};
		m_registry.execute(updateAutoContainers);

		m_registry.clearComponent<BoundingRectangleNeedsUpdate>();
	}

	void Hud::draw()
	{
		m_registry.execute(operations::DrawSprites2D(m_spriteRenderer));
		m_spriteRenderer.present(m_camera);
		m_spriteRenderer.clear();

		if (m_fontRenderer)
		{
			m_fontRenderer->clearText();
			m_registry.execute(operations::DrawText2D(*m_fontRenderer));
			m_fontRenderer->present(m_camera);
		}
	}

	void Hud::processInputs()
	{
		glm::vec2 cursorPos = input::InputManager::getCursorPos();
		cursorPos.y = m_registry.getComponentUnsafe<BoundingRectangle>(m_this).size.y - cursorPos.y;

		if (m_cursor)
		{
			m_registry.getComponentUnsafe<Transform2D>(m_cursor).position = cursorPos;
		}

		if (input::InputManager::isButtonPressed(input::MouseButton::LEFT))
		{
			m_registry.execute(ClickButton(cursorPos));
		}
	}

	glm::vec2 Hud::getAbsolutePosition(Entity _entity, glm::vec2 _relativePosition) const
	{
		const auto& pos = m_registry.getComponentUnsafe<Position2D>(_entity);
		const auto& rect = m_registry.getComponentUnsafe<BoundingRectangle>(_entity);

		const glm::vec2 min = pos.value - rect.alignment * rect.size;
		return min + _relativePosition * rect.size;
	}

	void Hud::createCursor(const graphics::Sprite& _sprite)
	{
		if (m_cursor) m_registry.erase(m_cursor);

		m_cursor = m_registry.create();
		m_registry.addComponent<Transform2D>(m_cursor, glm::vec2(0.f));
		m_registry.addComponent<Sprite>(m_cursor, _sprite, glm::vec3(0.f, 0.f, 0.0f), glm::vec2(0.f, 0.f));
	}
}