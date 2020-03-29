#include "drawModels.hpp"

namespace game {
namespace actions {

	void DrawModelsImpl::operator()(const Model& _model, const Transform& _transform)
	{
		m_renderer.draw(*_model.mesh, m_camera.view * _transform.value * _model.transform, m_camera.projection);
	}

	void DrawModelsImpl::present()
	{
		m_renderer.present();
		m_renderer.clear();
	}
}}
