#pragma once

#include "../blockalloc.hpp"
#include "../../math/geometrictypes.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <concepts>

namespace Utils {

	// Sparse Octree for axis aligned bounding boxes.
	template<typename T, int Dim, typename FloatT>
	//requires requires(VecT v) { {v[0] }->std::convertable_to<FloatT> }
	class SparseOctree
	{
	public:
		using AABB = math::AABB<Dim, FloatT>;
		using VecT = glm::vec<Dim, FloatT, glm::defaultp>;
		
		SparseOctree() { initRoot(); }

		void insert(const AABB& _boundingBox, const T& el)
		{
			// enlarge top
			AABB curBox = m_rootNode->box;
			FloatT size = m_size;
			while (!IsIn(_boundingBox, m_rootNode->box))
			{
				int index = 0;
				const VecT dif = curBox.max - curBox.min;
				for (int i = 0; i < Dim; ++i)
				{
					if (curBox.min[i] > _boundingBox.min[i])
					{
						curBox.min[i] -= dif[i];
						index += 1 << i;
					}
					else
						curBox.max[i] += dif[i];
				}
				Node* newRoot = m_allocator.create(curBox);
				newRoot->childs[index] = m_rootNode;
				m_rootNode = newRoot;
			}
			m_rootNode->insert(_boundingBox, el, m_allocator);
		/*	Node* n = m_rootNode;
			while(true)
			{
				constexpr FloatT MinSize = 1.0 / (2 << 3);
				const AABB& box = n->box;

				if (box.max[0] - box.min[0] <= MinSize)
				{
					n->elements.emplace_back(_boundingBox, el);
					return;
				}

				const VecT center = box.min + (box.max - box.min) * 0.5f;
				AABB newBox;
				int index = 0;
				for (int i = 0; i < Dim; ++i)
				{
					if (_boundingBox.min[i] < center[i] && _boundingBox.max[i] >= center[i])
					{
						n->elements.emplace_back(_boundingBox, el);
						return;
					}

					if (_boundingBox.min[i] > center[i] && _boundingBox.max[i] > center[i])
					{
						index += 1 << i;
						newBox.min[i] = center[i];
						newBox.max[i] = box.max[i];
					}
					else
					{
						newBox.min[i] = box.min[i];
						newBox.max[i] = center[i];
					}
				}

				if (!n->childs[index]) n->childs[index] = m_allocator.Create(newBox);
				n = n->childs[index];
			}*/
		}
		
		// remove all elements
		void clear()
		{
			m_allocator.reset();
			initRoot();
		}

		/* Interface of the Processor
			struct TreeProcessor
			{
				bool descend(const AABB& currentBox);
				void process(const AABB& key, T& el);
			};
		*/
		template<class Processor>
		void traverse(Processor& proc)
		{
			m_rootNode->traverse(proc);
		}
		// Example for a processor
		struct AABBQuery
		{
			AABBQuery(const AABB& _aabb) : aabb(_aabb) {}

			AABB aabb;
			std::vector<T> hits;

			bool descend(const AABB& currentBox)
			{
				return aabb.intersect(currentBox);
			}
			void process(const AABB& key, T& el)
			{
				if (aabb.intersect(key)) hits.push_back(el);
			}
		};
	
	private:
		void initRoot()
		{
			m_rootNode = m_allocator.create(AABB());
			for (int i = 0; i < Dim; ++i)
			{
				m_rootNode->box.min[i] = 0;
				m_rootNode->box.max[i] = 1;
			}
		}
	/*	template<int Ind, typename V>
			requires requires (V v) { v[Ind]; }
		auto Get(V& v) ->decltype(v[Ind]) { return v[Ind]; }

		template<int Ind, typename V>
			requires requires (V v) { v.x; }
		auto Get(V& v) -> decltype(*&v.x)&
		{
			if constexpr (Ind == 0) return v.x;
			if constexpr (Ind == 1) return v.y;
			if constexpr (Ind == 2) return v.z;
		}*/

		struct Node
		{
			Node(const AABB& _box)
				: box{_box}, childs{}
			{}

			void insert(const AABB& _boundingBox, const T& el, BlockAllocator<Node, 128>& _allocator)
			{
				constexpr FloatT MinSize = 1.0 / (2 << 3);

				if (box.max[0] - box.min[0] <= MinSize)
				{
					elements.emplace_back(_boundingBox, el);
					return;
				}

				const VecT center = box.min + (box.max - box.min)*0.5f;
				AABB newBox;
				int index = 0;
				for (int i = 0; i < Dim; ++i)
				{
					if (_boundingBox.min[i] < center[i] && _boundingBox.max[i] >= center[i])
					{
						elements.emplace_back(_boundingBox, el);
						return;
					}
					
					if (_boundingBox.min[i] > center[i] && _boundingBox.max[i] > center[i])
					{
						index += 1 << i;
						newBox.min[i] = center[i];
						newBox.max[i] = box.max[i];
					}
					else
					{
						newBox.min[i] = box.min[i];
						newBox.max[i] = center[i];
					}
				}

				if (!childs[index]) childs[index] = _allocator.create(newBox);
				childs[index]->insert(_boundingBox, el, _allocator);
			}

			template<typename Proc>
			void traverse(Proc& _proc)
			{
				if (!_proc.descend(box)) return;

				for (auto& [key, val] : elements)
					_proc.process(key, val);
				for (int i = 0; i < (1 << Dim); ++i)
					if (childs[i]) childs[i]->traverse(_proc);
			}

			std::vector< std::pair<AABB, T>> elements;
			AABB box;
			Node* childs[1 << Dim];
		};

		static bool IsIn(const AABB& _key, const AABB& _box)
		{
			for (int i = 0; i < Dim; ++i)
			{
				if (_box.min[i] > _key.min[i] || _box.max[i] < _key.max[i]) return false;
			}
			return true;
		}

		static int ChildIndex(const VecT& _key, const VecT& _center)
		{
			int ind = 0;
			for (int i = 0; i < Dim; ++i)
				if (center[i] < _key[i]) ind += 1 << i;
		}

		BlockAllocator<Node, 128> m_allocator;
		Node* m_rootNode;
		FloatT m_size;
	};


	// ********************************************************************* //
	// implementation
	// ********************************************************************* //

	/*template<typename T, int BS>
	void SparseOctree<T, BS>::insert(const pmp::vec3& key, const T& el)
	{
		Key center = m_rootNode->center_;
		FloatT size = m_rootNode->size_;
		while (!is_in(key, center, size))
		{
			int index = child_index(center, key);
			center -= CENTER_SHIFTS[index] * size;
			size *= 2;

			TreeNode* newRoot = m_allocator.create(center, size);
			newRoot->childs_[index] = m_rootNode;
			m_rootNode = newRoot;
		}

		m_rootNode->insert(key, el, m_allocator);
	}*/
}