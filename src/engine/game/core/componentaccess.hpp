#pragma once

#include "../../utils/containers/weakSlotMap.hpp"
#include "../../utils/metaProgHelpers.hpp"
#include "weakcomponentvector.hpp"
#include "entity.hpp"

namespace game {

	// different storage types are selected based on the component type
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

	// general component storage access
	template<component_type T>
	class Access
	{
	public:
		using ComponentType = T;

		explicit Access(ComponentStorage<T>& _targetStorage)
			: m_targetStorage(_targetStorage)
		{}

		auto iterate() { return m_targetStorage.iterate<T>(); }
		auto iterate() const { return m_targetStorage.iterate<T>(); }

		bool has(Entity _ent) const { return m_targetStorage.contains(_ent.toIndex()); }

		T& getUnsafe(Entity _ent) requires data_component_type<T> { return m_targetStorage.template at<T>(_ent.toIndex()); }
		const T& getUnsafe(Entity _ent) const requires data_component_type<T> { return m_targetStorage.template at<T>(_ent.toIndex()); }

		T* get(Entity _ent) requires data_component_type<T>
		{
			return m_targetStorage.contains(_ent.toIndex()) ?
				&m_targetStorage.template at<T>(_ent.toIndex())
				: nullptr;
		}
		const T* get(Entity _ent) const requires data_component_type<T>
		{
			return m_targetStorage.contains(_ent.toIndex()) ?
				&m_targetStorage.template at<T>(_ent.toIndex())
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

	template<component_type T>
	using WriteAccess = Access<T>;
	template<component_type T>
	using ReadAccess = const Access<T>;

	// Holds a number of components and allows Action execution on a (sub)set of them
	template<typename... CompAccess>
	class ComponentTuple
	{
	public:
		ComponentTuple(CompAccess... _accessors) : m_components(_accessors...) {}

		// construction from other tuple
		template<typename... ParentComps>
		ComponentTuple(const ComponentTuple<ParentComps...>& _parentComps)
			: m_components(getComp<typename CompAccess::ComponentType>(_parentComps)...)
		{}

		// Execute an Action on all entities having the components
		// expected by Action::operator(...).
		template<typename Action>
		void execute(Action& _action) { executeUnpack(_action, utils::UnpackFunction(&Action::operator())); }
		// This explicit version is only needed to capture rvalues.
		template<typename Action>
		void execute(const Action& _action) { executeUnpack(_action, utils::UnpackFunction(&Action::operator())); }

		// not a member function because that would require ".template" everywhere
		template<typename Comp>
		friend auto getComp(const ComponentTuple<CompAccess...>& _tuple)
		{
			static_assert(utils::contains_type<ReadAccess<Comp>, CompAccess...>::value
				|| utils::contains_type<WriteAccess<Comp>, CompAccess...>::value,
				"Tuple does not contain the required component.");

			if constexpr (utils::contains_type<ReadAccess<Comp>, CompAccess...>::value)
				return std::get<ReadAccess<Comp>>(_tuple.m_components);
			else
				return std::get<WriteAccess<Comp>>(_tuple.m_components);

		}
	private:
		template<typename Action, typename Comp, typename... Comps>
		void executeUnpack(Action& _action, utils::UnpackFunction<std::remove_cv_t<Action>, Comp, Comps...>)
		{
			if constexpr (std::is_convertible_v<Comp, Entity>)
				executeImpl<true, Action, std::decay_t<Comps>...>(_action, std::make_index_sequence<sizeof...(Comps) - 1>{});
			else
				executeImpl<false, Action, std::decay_t<Comp>, std::decay_t<Comps>...>(_action, std::make_index_sequence<sizeof...(Comps)>{});
		}

		template<bool WithEnt, typename Action, component_type Comp, component_type... Comps, std::size_t... I>
		void executeImpl(Action& _action, std::index_sequence<I...>)
		{
			auto mainContainer = getComp<Comp>(*this).iterate();

			for (auto it = mainContainer.begin(); it != mainContainer.end(); ++it)
			{
				Entity ent(it.key());

				if ((getComp<Comps>(*this).has(ent) && ...))
				{
					if constexpr (WithEnt)
						_action(ent, it.value(), getComp<Comps>(*this).getUnsafe(ent) ...);
					else
						_action(it.value(), getComp<Comps>(*this).getUnsafe(ent) ...);
				}
			}
		}

		std::tuple<CompAccess...> m_components;
	};
}