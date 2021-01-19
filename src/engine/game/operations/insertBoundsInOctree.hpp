#pragma once

#include "../../utils/containers/octree.hpp"
#include "../core/entity.hpp"
#include "../components/simpleComponents.hpp"

namespace game{
namespace operations{

	class InsertBoundsInOctree
	{
	public:
		using octree_t = utils::SparseOctree<Entity,3,float>;
		InsertBoundsInOctree(octree_t& _octree)
			: m_octree(_octree) {}
		void operator()(Entity _ent, components::Position& _pos, components::BoundingBox& _bb, components::Ammunition&) const {
			math::AABB<3> aabb(_pos.value + _bb.min, _pos.value + _bb.max);
			m_octree.insert(aabb, _ent);
		}
	private:
		octree_t& m_octree;
	};
}
}
