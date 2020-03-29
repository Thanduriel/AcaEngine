#include "updateTransform.hpp"

namespace game {
namespace actions {

	void UpdateTransformPosition::operator()(Transform& _transform, const Position& _position) const
	{
		_transform[3] = glm::vec4(_position, 1.f);
	}
}}
