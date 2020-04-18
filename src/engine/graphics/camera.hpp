#pragma once

#include <glm/glm.hpp>

namespace graphics {

	class Camera
	{
	public:
		// Perspective
		// @param _fov in angle
		Camera(float _fov, float _zNear, float zFar);
		// Orthogonal
		Camera(glm::vec2 _size, float _zNear = 0.f, float zFar = 1.f);

		const glm::mat4& getView() const { return m_view; }
		const glm::mat4& getProjection() const { return m_projection; }
		const glm::mat4& getViewProjection() const { return m_viewProjection; }

		void setView(const glm::mat4& _view) { m_view = _view; computeViewProjection(); }
	private:
		void computeViewProjection();

		glm::mat4 m_projection;
		glm::mat4 m_view;
		glm::mat4 m_viewProjection;
	};
}