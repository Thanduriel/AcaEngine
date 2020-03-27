#include "updateTransform.hpp"

namespace game {
namespace actions {

	void UpdateTransformPositionImpl::operator()(Transform& _transform, const Position& _position)
	{
		_transform[3] = glm::vec4(_position, 1.f);
	}
}}