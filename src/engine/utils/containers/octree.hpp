#pragma once

#include "../blockalloc.hpp"
#include "../../math/geometrictypes.hpp"
#include <glm/glm.hpp>
#include <memory>
#include <vector>
#include <concepts>

namespace utils {

	// Sparse octree for axis aligned bounding boxes.
	template<typename T, int Dim, typename FloatT>
	class SparseOctree
	{
	public:
		using AABB = math::AABB<Dim, FloatT>;
		using VecT = glm::vec<Dim, FloatT, glm::defaultp>;
		
		/// @brief Construct a sparse octree with a single node.
		/// @param _rootSize The initial size of the outer bounding box.
		/// @param _minLevel Maximal number of subdivisions from _rootSize 
		/// before a box is stored even if it is smaller.
		SparseOctree(FloatT _rootSize = 1.f, int _minLevel = 8, int _maxLevel = 8);

		/// @brief Insert a new element into the tree. Does not check for duplicates.
		/// @details If the box lies outside the current tree the root is expanded first.
		/// @param _boundingBox The bounding box used to determine the proper location.
		/// @param _el The element to insert.
		void insert(const AABB& _boundingBox, const T& _el);

		/// @brief Remove an element from the tree.
		/// @param _boundingBox The box used to search for the element.
		/// @param _el The element to remove.
		/// @return True if the element was found.
		bool remove(const AABB& _boundingBox, const T& _el);
		
		/// @brief Remove all elements from the tree.
		void clear()
		{
			m_allocator.reset();
			initRoot(m_size);
		}

		/* Interface of the Processor
			struct TreeProcessor
			{
				bool descend(const AABB& currentBox);
				void process(const AABB& key, T& el);
			};
		*/
		template<class Processor>
		void traverse(Processor& proc) const
		{
			m_rootNode->traverse(proc);
		}

		/// @brief Processor which invokes Op on all elements which overlap 
		//		   with the given AABB.
		template<typename Op>
		struct IntersectQuery
		{
			IntersectQuery(const AABB& _aabb, Op _op) : aabb(_aabb), operation(_op) {}

			AABB aabb;
			Op operation;

			bool descend(const AABB& currentBox) const
			{
				return aabb.intersect(currentBox);
			}
			void process(const AABB& key, const T& el)
			{
				if (aabb.intersect(key))
					operation(key, el);
			}
		};

		const AABB& getRootAABB() const { return m_rootNode->box; }
	
	private:
		constexpr static FloatT MIN_SIZE = 1.0 / (2 << 3);

		void initRoot(FloatT _size)
		{
			m_rootNode = m_allocator.create(AABB());
			for (int i = 0; i < Dim; ++i)
			{
				m_rootNode->box.min[i] = 0;
				m_rootNode->box.max[i] = _size;
			}
		}

		struct Node
		{
			explicit Node(const AABB& _box) noexcept
				: box{_box}, childs{}
			{}

			void insert(const AABB& _boundingBox, const T& el, SparseOctree& _tree)
			{
				if (box.max[0] - box.min[0] <= _tree.m_minSize)
				{
					elements.emplace_back(_boundingBox, el);
					return;
				}

				const VecT center = box.min + (box.max - box.min) * static_cast<FloatT>(0.5);
				AABB newBox;
				int index = 0;
				for (int i = 0; i < Dim; ++i)
				{
					if (_boundingBox.min[i] < center[i] && _boundingBox.max[i] > center[i])
					{
						elements.emplace_back(_boundingBox, el);
						return;
					}
					
					if (_boundingBox.min[i] >= center[i])
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

				if (!childs[index]) childs[index] = _tree.m_allocator.create(newBox);
				childs[index]->insert(_boundingBox, el, _tree);
			}

			// Search in the tree rooted at this node and remove the element if found.
			bool remove(const AABB& _boundingBox, const T& el, SparseOctree& _tree)
			{
				if (box.max[0] - box.min[0] <= _tree.m_minSize)
				{
					return remove(el);
				}

				const VecT center = box.min + (box.max - box.min) * static_cast<FloatT>(0.5);
				AABB newBox;
				int index = 0;
				for (int i = 0; i < Dim; ++i)
				{
					if (_boundingBox.min[i] < center[i] && _boundingBox.max[i] >= center[i])
					{
						return remove(el);
					}

					if (_boundingBox.min[i] >= center[i])
					{
						index += 1 << i;
					}
				}

				if (childs[index]) 
					return childs[index]->remove(_boundingBox, el, _tree);

				return false;
			}

			// Remove element from this node.
			bool remove(const T& el)
			{
				auto it = std::find_if(elements.begin(), elements.end(), [&](const std::pair<AABB, T>& _el)
				{
					return _el.second == el;
				});

				if (it == elements.end())
					return false;

				elements.erase(it);
				return true;
			}

			template<typename Proc>
			void traverse(Proc& _proc) const
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

		static bool isIn(const AABB& _key, const AABB& _box)
		{
			for (int i = 0; i < Dim; ++i)
			{
				if (_box.min[i] > _key.min[i] || _box.max[i] <= _key.max[i]) return false;
			}
			return true;
		}

		BlockAllocator<Node, 128> m_allocator;
		Node* m_rootNode;
		FloatT m_size; // initial root size
		FloatT m_minSize;
		FloatT m_maxSize;
	};


	// ********************************************************************* //
	// implementation
	// ********************************************************************* //

	template<typename T, int Dim, typename FloatT>
	SparseOctree<T, Dim, FloatT>::SparseOctree(FloatT _rootSize, int _minLevel, int _maxLevel)
		: m_size(_rootSize)
		, m_minSize(_rootSize / (2 << _minLevel))
		, m_maxSize(_rootSize * (2 << _maxLevel))
	{ 
		initRoot(_rootSize); 
	}

	template<typename T, int Dim, typename FloatT>
	void SparseOctree<T,Dim,FloatT>::insert(const AABB& _boundingBox, const T& el)
	{
		// enlarge top
		AABB curBox = m_rootNode->box;
		while (!isIn(_boundingBox, m_rootNode->box) 
			&& curBox.max[0] - curBox.min[0] < m_maxSize)
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
		m_rootNode->insert(_boundingBox, el, *this);
	}

	template<typename T, int Dim, typename FloatT>
	bool SparseOctree<T, Dim, FloatT>::remove(const AABB& _boundingBox, const T& el)
	{
		return m_rootNode->remove(_boundingBox, el, *this);
	}


}