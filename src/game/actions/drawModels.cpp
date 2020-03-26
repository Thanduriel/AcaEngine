#include "drawModels.hpp"

namespace game {
namespace actions {

	void DrawModelsImpl::operator()(Entity _ent, const Model& _mesh)
	{
		m_renderer.draw(*_mesh.mesh, m_viewProjection * _mesh.transform);
	}

	void DrawModelsImpl::present()
	{
		m_renderer.present();
		m_renderer.clear();
	}
}}