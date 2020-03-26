#pragma once

#include "utils/assert.hpp"
#include <vector>
#include <limits>
#include <utility>

namespace utils {
	template<typename Key, typename Value>
	class SlotMap
	{
		constexpr static Key INVALID_SLOT = std::numeric_limits<Key>::max();
	public:
		template<typename... Args>
		void emplace(Key _key, Args&&... _args)
		{
			// increase slots if necessary
			if (m_slots.size() <= _key)
				m_slots.resize(_key + 1, INVALID_SLOT);
			m_slots[_key] = static_cast<Key>(m_values.size());

			m_values.emplace_back(std::forward<Args>(_args)...);
			m_valuesToSlots.emplace_back(_key);
		}

		void erase(Key _key)
		{
			Assert(contains(_key), "Trying to delete an not existing element.");

			m_values[_key] = std::move(m_values.back());
			m_values.pop_back();
			m_valuesToSlots[_key] = m_valuesToSlots.back();
			m_valuesToSlots.pop_back();
		}

		// iterators
		class Iterator
		{
		public:
			Iterator(SlotMap& _target, std::size_t _ind) : m_target(_target), m_index(_ind) {}

			Key key() const { return m_target.m_valuesToSlots[m_index]; }
			Value& value() { return m_target.m_values[m_index]; }

			void operator++() { ++m_index; }
			bool operator!=(Iterator& _oth) const { return m_index != _oth.m_index; }
		private:
			std::size_t m_index;
			SlotMap& m_target;
		};
		auto begin() { return Iterator(*this, 0); }
		auto end() { return Iterator(*this, m_values.size()); }

		// access operations
		bool contains(Key _key) const { return _key < m_slots.size() && m_slots[_key] != INVALID_SLOT; }
		Value& operator[](Key _key) { return m_values[m_slots[_key]]; }
		const Value& operator[](Key _key) const { return m_values[m_slots[_key]]; }

		std::size_t size() const { return m_values.size(); }
	private:
		std::vector<Key> m_slots;
		std::vector<Key> m_valuesToSlots;
		std::vector<Value> m_values;
	};
}