#include "drawModels.hpp"
#include <spdlog/spdlog.h>

namespace game {
namespace operations {

	void DrawModels::operator()(const components::Model& _model, const components::Transform& _transform, const components::BoundingBox& _bb, const components::Position& _pos)
	{

		game::operations::GrepPointLights::octree_t::AABBQuery light_query({_pos.value + _bb.min, _pos.value + _bb.max});
		m_light_tree->traverse(light_query);
		m_renderer.draw(*_model.mesh, *_model.texture, m_camera->getView() * _transform.value * _model.transform, m_camera->getProjection(), {_pos.value, light_query.hits});
	}

	void DrawModels::present()
	{
		m_renderer.present(-glm::vec3(m_camera->getView()[3])/m_camera->getView()[3][3]);
		m_renderer.clear();
	}
}}
