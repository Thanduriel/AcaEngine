#pragma once

#include "../core/entity.hpp"
#include "../core/lifetimeManager.hpp"
#include "../components/simpleComponents.hpp"
#include "../operations/insertBoundsInOctree.hpp"

namespace game {
namespace operations {

	template<typename Manager>
	class DestroyHits
	{
	public:
		using octree_t = InsertBoundsInOctree::octree_t;
		/**
		 * @brief Destroy all Objects in octree which are colided with an Ammonition object 
		 */
		DestroyHits(Manager& _manager, octree_t& _octree)
			: m_manager{_manager}, m_octree{_octree} {}
		void operator()(Entity _ent, components::Position& _pos, components::BoundingBox& _bb, components::Ammonition&) const
		{
			math::AABB<3> aabb(_pos.value + _bb.min, _pos.value + _bb.max);
			octree_t::AABBQuery query(aabb);
			m_octree.traverse(query);
			for ( Entity hit : query.hits) {
				m_manager.destroy(hit);
			}
		}
	private:
		Manager& m_manager;
		octree_t& m_octree; ///< octree including targets
	};
}
}
