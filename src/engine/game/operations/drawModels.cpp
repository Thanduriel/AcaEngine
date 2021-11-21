#include "drawModels.hpp"

namespace game {
namespace operations {

	void DrawModels::operator()(const components::Model& _model, const components::Transform& _transform)
	{
		m_renderer.draw(*_model.mesh, *_model.texture, _transform.value * _model.transform);
	}

	void DrawModels::present(const graphics::Camera& _camera)
	{
		m_renderer.present(_camera);
		m_renderer.clear();
	}
}}
