#pragma once

#include "game/core/entity.hpp"
#include "game/core/action.hpp"
#include "game/components/model.hpp"
#include "graphics/renderer/meshrenderer.hpp"

namespace game {
namespace actions {

	class DrawModelsImpl
	{
	public:
		void operator()(Entity _ent, const Model& _mesh);

		void present();

		void setViewProjection(const glm::mat4& _viewProjection) { m_viewProjection = _viewProjection; }
	private:
		glm::mat4 m_viewProjection;
		graphics::MeshRenderer m_renderer;
	};
	
	using DrawMeshes = Action<DrawModelsImpl, Model>;
}}