#pragma once

#include "component.hpp"
#include "../../utils/metaProgHelpers.hpp"
#include "../../utils/containers/iteratorrange.hpp"
#include <vector>
#include <memory>

namespace game {

	// Type erased component vector.
	template<std::integral Key>
	class WeakComponentVector
	{
	public:
		using SizeType = size_t;

		template<typename Comp>
		WeakComponentVector(utils::TypeHolder<Comp>, size_t _initialCapacity = 4)
			: m_clear(clear<Comp>),
			m_components(new char[_initialCapacity * sizeof(Comp)])
		{
			m_entities.reserve(_initialCapacity);
		}

		WeakComponentVector(WeakComponentVector&& _oth) noexcept
			: m_clear(_oth.m_clear),
			m_entities(std::move(_oth.m_entities)),
			m_components(std::move(_oth.m_components))
		{
		}

		WeakComponentVector& operator=(WeakComponentVector&& _oth) noexcept
		{
			m_clear = _oth.m_clear;
			m_entities = std::move(_oth.m_entities);
			m_components = std::move(_oth.m_components);

			return *this;
		}

		~WeakComponentVector()
		{
			m_clear(*this);
		}

		template<typename Value>
		const Value& get(size_t _pos) const { return *(reinterpret_cast<const Value*>(m_components.get()) + _pos); }

		template<typename Value>
		Value& get(size_t _pos) { return *(reinterpret_cast<Value*>(m_components.get()) + _pos); }

		template<component_type Component, typename... Args>
		Component& emplace(Key _key, Args&&... _args)
		{
			const size_t prevCapacity = m_entities.capacity();
			m_entities.push_back(_key);
			const size_t capacity = m_entities.capacity();
			if (prevCapacity != capacity)
			{
				char* newBuf = new char[m_entities.capacity() * sizeof(Component)];
				for (size_t i = 0; i < prevCapacity; ++i)
				{
					new(&newBuf[i * sizeof(Component)]) Component(std::move(get<Component>(i)));
					get<Component>(i).~Component();
				}
				m_components.reset(newBuf);
			}

			Component& comp = *new(&get<Component>(m_entities.size() - 1)) Component(std::forward<Args>(_args)...);
			return comp;
		}

		size_t size() const { return m_entities.size(); }

		void clear()
		{
			m_clear(*this);
		}

		template<typename Value>
		struct Accessor
		{
			static Key key(const WeakComponentVector& _this, SizeType _index) { return _this.m_entities[_index]; }
			static Value& value(WeakComponentVector& _this, SizeType _index) { return _this.get<Value>(_index); }
			static const Value& value(const WeakComponentVector& _this, SizeType _index) { return _this.get<Value>(_index); }
		};
		template<typename Value>
		auto iterate() { return utils::IteratorRange<WeakComponentVector, Key, Value, Accessor<Value>>(*this); }
		template<typename Value>
		auto iterate() const { return utils::ConstIteratorRange<WeakComponentVector, Key, Value, Accessor<Value>>(*this); }
	private:
		template<typename Value>
		static void clear(WeakComponentVector& _container)
		{
			for (size_t i = 0; i < _container.m_entities.size(); ++i)
			{
				_container.get<Value>(i).~Value();
			}

			_container.m_entities.clear();
		}

		using Clear = void(*)(WeakComponentVector&);
		Clear m_clear;

		std::vector<Key> m_entities;
		std::unique_ptr<char[]> m_components;
	};
}