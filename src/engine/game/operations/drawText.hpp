#pragma once

#include "../components/components2D.hpp"
#include "../components/simpleComponents.hpp"

namespace graphics {
	class FontRenderer;
}

namespace game {
	namespace operations {

		class DrawText
		{
		public:
			DrawText(graphics::FontRenderer& _renderer) : m_renderer(_renderer) {}

			void operator()(const components::Label& _label, const components::Position& _position) const;

		private:
			graphics::FontRenderer& m_renderer;
		};

		class DrawText2D
		{
		public:
			DrawText2D(graphics::FontRenderer& _renderer) : m_renderer(_renderer) {}

			void operator()(const components::Label& _label,
				const components::Transform2D& _transform) const;

		private:
			graphics::FontRenderer& m_renderer;
		};

	}
}