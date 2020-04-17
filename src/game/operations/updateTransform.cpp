#include "updateTransform.hpp"

namespace game {
namespace operations {

	void UpdateTransformPosition::operator()(components::Transform& _transform, const components::Position& _position) const
	{
		_transform.value[3] = glm::vec4(_position.value, 1.f);
	}

	void UpdateTransformPosition2D::operator()(components::Transform2D& _transform, const components::Position2D& _position) const
	{
		_transform.value[2] = glm::vec3(_position.value, 1.f);
	}

	void UpdateTransformRotation2D::operator()(components::Transform2D& _transform, const components::Rotation2D& _rotation) const
	{
		const float cosTheta = glm::cos(_rotation.value);
		const float sinTheta = glm::sin(_rotation.value);

		_transform.value[0][0] = cosTheta;
		_transform.value[0][1] = sinTheta;
		_transform.value[1][0] = -sinTheta;
		_transform.value[1][1] = cosTheta;

	}
}}
