#pragma once

#include <utility>
#include <memory>

namespace utils {
	/// @brief 
	/// @tparam T 
	template<typename T, int ElemPerBlock>
	class BlockAllocator
	{
	public:
		BlockAllocator()
			: first(new Node()), current(first.get())
		{

		}

		// Create a new object.
		template<typename... Args>
		T* create(Args&&... args)
		{
			if (current->numElements == ElemPerBlock)
			{
				Node* prev = current;
				current = new Node();
				prev->next = current;
			}
			T* ptr = reinterpret_cast<T*>(current->buffer) + current->numElements;
			++current->numElements;

			return new(ptr)T(std::forward<Args>(args)...);
		}

		// Delete all objects and free all but one block.
		void reset()
		{
			first.reset(new Node());
			current = first.get();
		}

	private:
		struct Node
		{
			~Node()
			{
				T* ptr = reinterpret_cast<T*>(buffer);
				for (int i = 0; i < numElements; ++i)
					ptr[i].~T();

				if (next) delete next;
			}

			char buffer[sizeof(T) * ElemPerBlock];
			Node* next = nullptr;
			int numElements = 0;
		};

		std::unique_ptr<Node> first;
		Node* current;
	};
}