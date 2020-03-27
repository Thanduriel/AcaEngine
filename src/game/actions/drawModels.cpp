#include "drawModels.hpp"

namespace game {
namespace actions {

	void DrawModelsImpl::operator()(const Model& _mesh, const Transform& _transform)
	{
		m_renderer.draw(*_mesh.mesh, m_viewProjection * _transform * _mesh.transform);
	}

	void DrawModelsImpl::present()
	{
		m_renderer.present();
		m_renderer.clear();
	}
}}