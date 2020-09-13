#include "drawText.hpp"
#include "../../graphics/renderer/fontrenderer.hpp"

namespace game {
	namespace operations {

		void DrawText::operator()(const components::Label& _label, const components::Position& _position) const
		{
			m_renderer.draw(
				_label.position + _position.value,
				_label.text.c_str(),
				_label.fontSize,
				_label.color,
				_label.rotation,
				_label.alignment.x,
				_label.alignment.y);
		}

		void DrawText2D::operator()(const components::Label& _label,
			const components::Position2D& _position,
			const components::Rotation2D& _rotation) const
		{
			m_renderer.draw(
				_label.position + glm::vec3(_position.value, 0.f),
				_label.text.c_str(),
				_label.fontSize,
				_label.color,
				_label.rotation + _rotation.value,
				_label.alignment.x,
				_label.alignment.y);
		}
	}
}