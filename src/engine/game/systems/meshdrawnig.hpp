#pragma once

#include <engine/graphics/renderer/meshrenderer.hpp>
#include <engine/game/core/componentaccess.hpp>
#include <engine/game/components/simpleComponents.hpp>
#include <engine/game/components/model.hpp>

namespace game {
namespace systems {

	class MeshDrawing
	{
	public:
		using Components = ComponentTuple<
			ReadAccess<components::Transform>,
			ReadAccess<components::Model>>;

		void update(Components _comps, const graphics::Camera& _camera);
	private:
		graphics::MeshRenderer m_meshRenderer;
	};
}
}