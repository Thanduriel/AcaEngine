#include "updateTransform.hpp"

namespace game {
namespace actions {

	void UpdateTransformPosition::operator()(components::Transform& _transform, const components::Position& _position) const
	{
		_transform.value[3] = glm::vec4(_position.value, 1.f);
	}
}}
