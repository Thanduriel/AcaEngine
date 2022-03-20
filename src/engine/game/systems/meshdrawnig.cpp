#include "meshdrawnig.hpp"

namespace game {
namespace systems{

	void MeshDrawing::update(Components _comps, const graphics::Camera& _camera)
	{
		m_meshRenderer.clear();

		_comps.execute([&](const components::Model& model, const components::Transform& transform)
			{
				m_meshRenderer.draw(*model.mesh, *model.texture, transform.value * model.transform);
			});

		m_meshRenderer.present(_camera);
	}
}
}