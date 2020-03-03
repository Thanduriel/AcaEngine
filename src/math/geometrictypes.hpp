#pragma once

#include "utils/assert.hpp"

#include <glm/glm.hpp>
#include <cstdint>

namespace math {

	struct Rectangle
	{
		glm::vec2 min;
		glm::vec2 max;

		/// \brief Create uninitialized rectangle.
		Rectangle() noexcept {}

		/// \brief Construct from minimal and maximal coordinates
		Rectangle(const glm::vec2& _min, const glm::vec2& _max) noexcept :
			min(_min),
			max(_max)
		{
			ASSERT(_min.x <= _max.x && _min.y <= _max.y,
				"Minimum coordinates must be smaller or equal the maximum.");
		}

		/// \brief Create an optimal box for a set of points
		Rectangle(const glm::vec2* _points, uint32_t _numPoints) noexcept
		{
			ASSERT(_points && _numPoints > 0, "The point list must have at least one point.");
			min = max = *_points++;
			for (uint32_t i = 1; i < _numPoints; ++i, ++_points)
			{
				min = glm::min(min, *_points);
				max = glm::max(max, *_points);
			}
		}
	};
}