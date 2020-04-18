#pragma once

#include "../core/entity.hpp"
#include "../components/model.hpp"
#include "../components/simpleComponents.hpp"
#include "../../graphics/renderer/meshrenderer.hpp"
#include "../../graphics/camera.hpp"

namespace game {
namespace operations {

	class DrawModels
	{
	public:
		void operator()(const components::Model& _mesh, const components::Transform& _transform);

		void present();

		void setCamera(const graphics::Camera& _camera) 
		{ 
			m_camera = &_camera;
		}
	private:
		const graphics::Camera* m_camera;
		graphics::MeshRenderer m_renderer;
	};
}}