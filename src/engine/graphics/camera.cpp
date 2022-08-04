#include "camera.hpp"
#include "core/device.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace graphics {

	using namespace glm;

	Camera::Camera(float _fov, float _zNear, float zFar)
		: m_projection(glm::perspective(glm::radians(_fov), Device::getAspectRatio(), _zNear, zFar)),
		m_view(glm::identity<glm::mat4>())
	{
		updateMatrices();
	}

	Camera::Camera(glm::vec2 _size, glm::vec2 _origin, float _zNear, float _zFar)
		: m_projection(glm::ortho(-_size.x*_origin.x, _size.x*(1.f-_origin.x), 
			-_size.y * _origin.y, _size.y* (1.f - _origin.y), 
			_zNear, _zFar)),
		m_view(glm::identity<glm::mat4>())
	{
		updateMatrices();
	}

	vec3 Camera::toWorldSpace(const vec2& _screenSpace) const
	{
		return toWorldSpace(_screenSpace, -1.f);
	}

	math::Ray3D Camera::getRay(const glm::vec2& _screenSpace) const
	{
		const glm::vec4 origin4 = m_viewProjectionInv * glm::vec4(0.f, 0.f, 0.f, 1.f);
		const glm::vec3 origin = glm::vec3(origin4) / origin4.w;
		// far plane point here should not be necessary? but currently it is
		const glm::vec3 dest = toWorldSpace(_screenSpace, 1.f);
		return math::Ray3D(origin, glm::normalize(dest - origin));
	}

	vec3 Camera::toWorldSpace(const vec2& _screenSpace, float _z) const
	{
		vec2 clipSpace = _screenSpace / vec2(Device::getBufferSize());
		clipSpace.y = 1.f - clipSpace.y;
		clipSpace = clipSpace * 2.f - vec2(1.f, 1.f);

		const vec4 worldSpace = m_viewProjectionInv * vec4(clipSpace, _z, 1.f);

		return vec3(worldSpace) / worldSpace.w;
	}

	void Camera::updateMatrices()
	{
		m_viewProjection = m_projection * m_view;
		m_viewProjectionInv = glm::inverse(m_viewProjection);
	}
}