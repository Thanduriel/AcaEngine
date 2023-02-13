#pragma once

#include <glm/vec4.hpp>

namespace graphics {
	// Standard vec4 can be used as a flexible color class.
	using Color = glm::vec4;

	// Color with 8 bit per channel meant for efficient storage and rendering. 
	class PackedColor
	{
	public:
		PackedColor(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255);
		PackedColor(const Color& _color);
		PackedColor(uint32_t _color);

		bool operator==(const PackedColor&) const = default;

		operator uint32_t() const noexcept;
		operator Color() const noexcept;
	private:
		uint32_t m_data;
	};
}