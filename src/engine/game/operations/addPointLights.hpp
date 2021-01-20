#pragma once

#include "engine/game/components/lights.hpp"
#include "engine/utils/containers/octree.hpp"
#include "engine/game/components/simpleComponents.hpp"
#include "engine/math/geometrictypes.hpp"
#include <spdlog/spdlog.h>
#include <vector>


namespace game { namespace operations {
	class GrepPointLights
	{
	public:
		struct PointLight {
			glm::vec3 color;
			glm::vec3 position;
			float intensity;
		};
		using octree_t = utils::SparseOctree<PointLight, 3, float>;
		GrepPointLights(octree_t& _octree)
		: m_octree{_octree}{}

		void operator()(const components::Position& _pos, const components::BoundingBox& _bb, const components::PointLight& _pl) const {
			math::AABB<3,float> aabb(_pos.value + _bb.min, _pos.value + _bb.max);
			m_octree.insert( aabb, PointLight{ .color = _pl.color, .position = _pos.value,.intensity = _pl.intensity } );
		}
	private:
		octree_t& m_octree; ///< octree containing id and bounds of ligths
	};
} } // end namespace operations game
