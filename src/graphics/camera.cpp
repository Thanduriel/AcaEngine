#include "camera.hpp"
#include <graphics/core/device.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace graphics {

	Camera::Camera(float _fov, float _zNear, float zFar)
		: m_projection(glm::perspective(glm::radians(_fov), Device::getAspectRatio(), _zNear, zFar)),
		m_view(glm::identity<glm::mat4>())
	{}

	Camera::Camera(glm::vec2 _size, float _zNear, float zFar)
		: m_projection(glm::ortho(0.0f, _size.x, 0.0f, _size.y, 0.f, 1.f)),
		m_view(glm::identity<glm::mat4>())
	{

	}

	void Camera::computeViewProjection()
	{
		m_viewProjection = m_projection * m_view;
	}
}