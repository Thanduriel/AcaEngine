#include "drawModels.hpp"

namespace game {
namespace actions {

	void DrawModels::operator()(const Model& _model, const Transform& _transform)
	{
		m_renderer.draw(*_model.mesh, m_camera.view * _transform.value * _model.transform, m_camera.projection);
	}

	void DrawModels::present()
	{
		m_renderer.present();
		m_renderer.clear();
	}
}}
