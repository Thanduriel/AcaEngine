#include "drawModels.hpp"

namespace game {
namespace operations {

	void DrawModels::operator()(const components::Model& _model, const components::Transform& _transform)
	{
		m_renderer.draw(*_model.mesh, *_model.texture, m_camera.view * _transform.value * _model.transform, m_camera.projection);
	}

	void DrawModels::present()
	{
		m_renderer.present();
		m_renderer.clear();
	}
}}
