#pragma once

#include "game/core/entity.hpp"
#include "game/core/action.hpp"
#include "game/components/model.hpp"
#include "game/components/simpleComponents.hpp"
#include "graphics/renderer/meshrenderer.hpp"
#include "graphics/camera.hpp"

namespace game {
namespace actions {

	class DrawModels
	{
	public:
		void operator()(const components::Model& _mesh, const components::Transform& _transform);

		void present();

		void setCamera(const graphics::Camera& _camera) 
		{ 
			m_camera = _camera;
		}
	private:
		graphics::Camera m_camera;
		graphics::MeshRenderer m_renderer;
	};
}}