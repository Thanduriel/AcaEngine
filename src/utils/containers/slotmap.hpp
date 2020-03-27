#pragma once

#include "utils/assert.hpp"
#include <vector>
#include <limits>
#include <utility>

namespace utils {
	template<typename Key, typename Value>
	class SlotMap
	{
	protected:
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
			ASSERT(contains(_key), "Trying to delete a not existing element.");

			const Key ind = m_slots[_key];
			m_slots[_key] = INVALID_SLOT;

			if (ind+1 < m_values.size())
			{
				m_values[ind] = std::move(m_values.back());
				m_slots[m_valuesToSlots.back()] = ind;
				m_valuesToSlots[ind] = m_valuesToSlots.back();
			}
			m_values.pop_back();
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
	protected:

		std::vector<Key> m_slots;
		std::vector<Key> m_valuesToSlots;
		std::vector<Value> m_values;
	};

	template<typename Key, typename Value>
	class MultiSlotMap : public SlotMap<Key, Value>
	{
	public:
		template<typename... Args>
		void emplace(Key _key, Args&&... _args)
		{
			if (contains(_key))
			{
				const Key ind = m_slots[_key];
				m_links.push_back({ind, INVALID_SLOT });
				m_links[ind].next = static_cast<Key>(m_values.size());
				// slot points to the latest component
				m_slots[_key] = m_links[ind].next;
			}
			else
				m_links.push_back({ INVALID_SLOT, INVALID_SLOT });
			SlotMap::emplace(_key, std::forward<Args>(_args)...);
		}

		// erases all components associated with this entity
		void erase(Key _key)
		{
			const Key ind = m_slots[_key];
			Key cur = ind;
			do{
				Key temp = m_links[cur].prev;
				eraseSlot(cur);
				cur = temp;

			} while (cur != INVALID_SLOT);

			m_slots[_key] = INVALID_SLOT;
		}
	private:
		void eraseSlot(Key _slot)
		{
			if (m_links[_slot].prev != INVALID_SLOT) m_links[m_links[_slot].prev].next = INVALID_SLOT;
			if (m_links[_slot].next != INVALID_SLOT) m_links[m_links[_slot].next].prev = INVALID_SLOT;

			if (_slot+1 < m_values.size())
			{
				m_values[_slot] = std::move(m_values.back());
				m_valuesToSlots[_slot] = m_valuesToSlots.back();

				const Link& link = m_links.back();
				if (link.prev != INVALID_SLOT) m_links[link.prev].next = _slot;
				if (link.next != INVALID_SLOT) m_links[link.next].prev = _slot;
				else m_slots[m_valuesToSlots.back()] = _slot;
				m_links[_slot] = m_links.back();
			}
			m_values.pop_back();
			m_valuesToSlots.pop_back();
			m_links.pop_back();
		}

		struct Link 
		{
			Key prev, next;
		};
		std::vector<Link> m_links;
	};
}