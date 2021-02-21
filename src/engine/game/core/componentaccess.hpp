#pragma once

#include "../../utils/containers/weakSlotMap.hpp"
#include "weakcomponentvector.hpp"

namespace game {

	using DataStorage = utils::WeakSlotMap<Entity::BaseType>;
	using MessageStorage = WeakComponentVector<Entity::BaseType>;

	namespace details {

		template<typename Val>
		struct StorageDecider { using type = DataStorage; };

		template<message_component_type Val>
		struct StorageDecider<Val> { using type = MessageStorage; };
	}

	template<typename Val>
	using ComponentStorage = typename details::StorageDecider<Val>::type;

	template<component_type T>
	class ReadAccess
	{
	public:
		explicit ReadAccess(const ComponentStorage<T>& _targetStorage)
			: m_targetStorage(_targetStorage)
		{}

		auto begin() const { return m_targetStorage.begin(); }
		auto end() const { return m_targetStorage.end(); }

		bool has(Entity _ent) const { return m_targetStorage.contains(_ent.toIndex()); }

		const T& getUnsafe(Entity _ent) const requires data_component_type<T> { return m_targetStorage.template at<T>(_ent.toIndex()); }

		const T* get(Entity _ent) const requires data_component_type<T>
		{
			return m_targetStorage.contains(_ent.toIndex()) ?
				&m_targetStorage.template at<T>(_ent.toIndex())
				: nullptr;
		}
	private:
		const ComponentStorage<T>& m_targetStorage;
	};

	template<component_type T>
	class WriteAccess
	{
	public:
		WriteAccess(ComponentStorage<T>& _targetStorage)
			: m_targetStorage(_targetStorage)
		{}

		auto begin() { return m_targetStorage.begin(); }
		auto end() { return m_targetStorage.end(); }

		T& getUnsafe(Entity _ent) requires data_component_type<T> { return m_targetStorage.template at<T>(_ent.toIndex()); }

		T* get(Entity _ent) requires data_component_type<T>
		{
			return m_targetStorage.contains(_ent.toIndex()) ?
				m_targetStorage.template at<T>(_ent.toIndex())
				: nullptr;
		}

		template< typename... Args>
		T& add(Entity _ent, Args&&... _args)
		{
			return m_targetStorage.template emplace<T>(_ent.toIndex(), std::forward<Args>(_args)...);
		}

		void clear() { m_targetStorage.clear(); }
	private:
		ComponentStorage<T>& m_targetStorage;
	};
}