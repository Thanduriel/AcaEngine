#pragma once

#include "../../utils/assert.hpp"
#include "../../utils/metaProgHelpers.hpp"
#include "iteratorrange.hpp"
#include <vector>
#include <limits>
#include <utility>
#include <concepts>
#include <memory>

namespace utils {
	template<std::integral Key, bool TrivialDestruct = false>
	class WeakSlotMap
	{
	protected:
		constexpr static Key INVALID_SLOT = std::numeric_limits<Key>::max();
	public:
		using SizeType = Key;

		template<std::movable Value>
		WeakSlotMap(utils::TypeHolder<Value>, SizeType _initialSize = 4)
			: m_elementSize(sizeof(Value)),
			m_destructor(destroyElement<Value>),
			m_move(moveElement<Value>),
			m_size(0),
			m_capacity(_initialSize),
			m_values(new char[index(m_capacity)])
		{
			static_assert(std::is_trivially_destructible_v<Value> || !TrivialDestruct,
				"Managed elements require a destructor call.");
		}

		WeakSlotMap(WeakSlotMap&& _oth) noexcept
			: m_elementSize(_oth.m_elementSize),
			m_destructor(_oth.m_destructor),
			m_move(_oth.m_move),
			m_size(_oth.m_size),
			m_capacity(_oth.m_capacity),
			m_values(std::move(_oth.m_values)),
			m_slots(std::move(_oth.m_slots)),
			m_valuesToSlots(std::move(_oth.m_valuesToSlots))
		{
			_oth.m_size = 0;
			_oth.m_capacity = 0;
		}

		WeakSlotMap& operator=(WeakSlotMap&& _oth) noexcept
		{
			m_elementSize = _oth.m_elementSize;
			m_destructor = _oth.m_destructor;
			m_move = _oth.m_move;
			m_size = _oth.m_size;
			m_capacity = _oth.m_capacity;
			m_values = std::move(_oth.m_values);
			m_slots = std::move(_oth.m_slots);
			m_valuesToSlots = std::move(_oth.m_valuesToSlots);

			_oth.m_size = 0;
			_oth.m_capacity = 0;

			return *this;
		}


		~WeakSlotMap()
		{
			destroyValues();
		}

		template<typename Value, typename... Args>
		Value& emplace(Key _key, Args&&... _args)
		{
			// increase slots if necessary
			if (m_slots.size() <= _key)
				m_slots.resize(_key + 1, INVALID_SLOT);
			else if(m_slots[_key] != INVALID_SLOT) // already exists
				return at<Value>(_key);

			m_slots[_key] = m_size;

			m_valuesToSlots.emplace_back(_key);
			if(m_capacity == m_size)
			{
				m_capacity = static_cast<SizeType>(1.5 * m_capacity);
				char* newBuf = new char[index(m_capacity)];
				for (SizeType i = 0; i < m_size; ++i)
				{
					new(&newBuf[i*m_elementSize]) Value(std::move(get<Value>(i)));
					get<Value>(i).~Value();
				}

				m_values.reset(newBuf);
			}
			
			return *new (&get<Value>(m_size++)) Value (std::forward<Args>(_args)...);
		}

		void erase(Key _key)
		{
			ASSERT(contains(_key), "Trying to delete a not existing element.");

			const Key ind = m_slots[_key];
			m_slots[_key] = INVALID_SLOT;
			char* back = &m_values[index(m_size - 1)];

			if (ind+1 < m_size)
			{
				m_move(&m_values[index(ind)], back);
				m_slots[m_valuesToSlots.back()] = ind;
				m_valuesToSlots[ind] = m_valuesToSlots.back();
			}

			--m_size;
			if constexpr (!TrivialDestruct) m_destructor(back);
			m_valuesToSlots.pop_back();
		}

		void clear()
		{
			m_slots.clear();
			m_valuesToSlots.clear();
			destroyValues();
			m_size = 0;
		}

		// iterators
		template<typename Value>
		struct Accessor
		{
			static Key key(const WeakSlotMap& _this, SizeType _index) { return _this.m_valuesToSlots[_index]; }
			static Value& value(WeakSlotMap& _this, SizeType _index) { return _this.get<Value>(_index); }
			static const Value& value(const WeakSlotMap& _this, SizeType _index) { return _this.get<Value>(_index); }
		};

		template<typename Value>
		auto iterate() { return IteratorRange<WeakSlotMap, Key, Value, Accessor<Value>>(*this); }
		template<typename Value>
		auto iterate() const { return ConstIteratorRange<WeakSlotMap, Key, Value, Accessor<Value>>(*this); }

		// access operations
		bool contains(Key _key) const { return _key < m_slots.size() && m_slots[_key] != INVALID_SLOT; }
		
		template<typename Value>
		Value& at(Key _key) { return reinterpret_cast<Value&>(m_values[index(m_slots[_key])]); }
		template<typename Value>
		const Value& at(Key _key) const { return reinterpret_cast<const Value&>(m_values[index(m_slots[_key])]); }

		SizeType size() const { return m_size; }
		bool empty() const { return m_size == 0; }
	protected:
		// access through internal index
		template<typename Value>
		Value& get(SizeType _ind) { return reinterpret_cast<Value&>(m_values[index(_ind)]); }
		template<typename Value>
		const Value& get(SizeType _ind) const { return reinterpret_cast<const Value&>(m_values[index(_ind)]); }
		size_t index(SizeType _ind) const { return static_cast<size_t>(_ind) * m_elementSize; }

		void destroyValues()
		{
			if constexpr (TrivialDestruct) return;

			for (SizeType i = 0; i < m_size; ++i)
				m_destructor(&m_values[index(i)]);
		}

		template<typename Value>
		static void destroyElement(void* ptr)
		{
			static_cast<Value*>(ptr)->~Value();
		}
		using Destructor = void(*)(void*);

		template<typename Value>
		static void moveElement(void* dst, void* src)
		{
			*static_cast<Value*>(dst) = std::move(*static_cast<Value*>(src));
		}
		using Move = void(*)(void*, void*);

		int m_elementSize;
		Destructor m_destructor;
		Move m_move;
		SizeType m_size;
		SizeType m_capacity;

		std::unique_ptr<char[]> m_values;
		std::vector<Key> m_slots;
		std::vector<Key> m_valuesToSlots;
	};
}
