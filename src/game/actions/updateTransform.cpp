#include "updateTransform.hpp"

namespace game {
namespace actions {

	void UpdateTransformPosition::operator()(Transform& _transform, const Position& _position) const
	{
		_transform.value[3] = glm::vec4(_position.value, 1.f);
	}
}}
