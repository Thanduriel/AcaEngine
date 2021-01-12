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
		 * @brief Destroy all Objects with CanExplode Flag which collids with object in octree
		 */
		DestroyHits(Manager& _manager, octree_t& _octree, int& _count)
			: m_manager{_manager}, m_octree{_octree}, m_count{_count} {}
		void operator()(Entity _ent, components::Position& _pos, components::BoundingBox& _bb, components::CanExplode&) const
		{
			math::AABB<3> aabb(_pos.value + _bb.min, _pos.value + _bb.max);
			octree_t::AABBQuery query(aabb);
			m_octree.traverse(query);
			if (query.hits.size()) {
				m_manager.destroy(_ent);
				++m_count;
			}
		}
	private:
		int& m_count;
		Manager& m_manager;
		octree_t& m_octree; ///< octree including targets
	};
}
}
