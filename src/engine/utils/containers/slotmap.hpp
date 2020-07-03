#pragma once

#include "../../utils/assert.hpp"
#include <vector>
#include <limits>
#include <utility>
#include <concepts>

namespace utils {
	template<std::integral Key, std::movable Value>
	class SlotMap
	{
	protected:
		constexpr static Key INVALID_SLOT = std::numeric_limits<Key>::max();
	public:
		template<typename... Args>
		Value& emplace(Key _key, Args&&... _args)
		{
			// increase slots if necessary
			if (m_slots.size() <= _key)
				m_slots.resize(_key + 1, INVALID_SLOT);
			m_slots[_key] = static_cast<Key>(m_values.size());

			m_valuesToSlots.emplace_back(_key);
			return m_values.emplace_back(std::forward<Args>(_args)...);
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

			Value& operator*() { return m_target.m_values[m_index]; }
			const Value& operator*() const { return m_target.m_values[m_index]; }

			Iterator& operator++() { ++m_index; return *this; }
			Iterator operator++(int) { Iterator tmp(*this);  ++m_index; return tmp; }
			bool operator==(const Iterator& _oth) const { ASSERT(&m_target == &_oth.m_target, "Comparing iterators of different containers."); return m_index == _oth.m_index; }
			bool operator!=(const Iterator& _oth) const { ASSERT(&m_target == &_oth.m_target, "Comparing iterators of different containers."); return m_index != _oth.m_index; }
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
		bool empty() const { return m_values.empty(); }
	protected:

		std::vector<Key> m_slots;
		std::vector<Key> m_valuesToSlots;
		std::vector<Value> m_values;
	};

	// Allows multiple values for the same Key to be stored.
	template<typename Key, typename Value>
	class MultiSlotMap : public SlotMap<Key, Value>
	{
		using Base = SlotMap<Key, Value>;
	public:
		template<typename... Args>
		Value& emplace(Key _key, Args&&... _args)
		{
			if (Base::contains(_key))
			{
				const Key ind = Base::m_slots[_key];
				m_links.push_back({ind, Base::INVALID_SLOT });
				m_links[ind].next = static_cast<Key>(Base::m_values.size());
				// slot points to the latest component
				Base::m_slots[_key] = m_links[ind].next;
			}
			else
				m_links.push_back({ Base::INVALID_SLOT, Base::INVALID_SLOT });
			
			return Base::emplace(_key, std::forward<Args>(_args)...);
		}

		// erases all components associated with this entity
		void erase(Key _key)
		{
			const Key ind = Base::m_slots[_key];
			Key cur = ind;
			do{
				Key temp = m_links[cur].prev;
				eraseSlot(cur);
				cur = temp;

			} while (cur != Base::INVALID_SLOT);

			Base::m_slots[_key] = Base::INVALID_SLOT;
		}
	private:
		void eraseSlot(Key _slot)
		{
			if (m_links[_slot].prev != Base::INVALID_SLOT) m_links[m_links[_slot].prev].next = Base::INVALID_SLOT;
			if (m_links[_slot].next != Base::INVALID_SLOT) m_links[m_links[_slot].next].prev = Base::INVALID_SLOT;

			if (_slot+1u < Base::m_values.size())
			{
				Base::m_values[_slot] = std::move(Base::m_values.back());
				Base::m_valuesToSlots[_slot] = Base::m_valuesToSlots.back();

				const Link& link = m_links.back();
				if (link.prev != Base::INVALID_SLOT) m_links[link.prev].next = _slot;
				if (link.next != Base::INVALID_SLOT) m_links[link.next].prev = _slot;
				else Base::m_slots[Base::m_valuesToSlots.back()] = _slot;
				m_links[_slot] = m_links.back();
			}
			Base::m_values.pop_back();
			Base::m_valuesToSlots.pop_back();
			m_links.pop_back();
		}

		struct Link 
		{
			Key prev, next;
		};
		std::vector<Link> m_links;
	};
}
