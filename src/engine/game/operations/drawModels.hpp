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
		void operator()(const components::Model& _mesh, const components::Transform& _transform, const components::BoundingBox& _bb, const components::Position& _pos);

		void present();

		void setCamera(const graphics::Camera& _camera) 
		{ 
			m_camera = &_camera;
		}

		void setLights(const operations::GrepPointLights::octree_t& _light_tree)
		{
			m_light_tree = &_light_tree;
		}
	private:
		const graphics::Camera* m_camera;
		const operations::GrepPointLights::octree_t* m_light_tree;
		graphics::MeshRenderer m_renderer;
	};
}}
