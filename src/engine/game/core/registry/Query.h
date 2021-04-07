/**
 * Project Untitled
 */


#pragma once

#include "types.h"

#include <unordered_map>

namespace registry {

	template<typename T>
	concept QueryTidIterator = requires (T t){{*t}->std::convertible_to<typeid_t>;};

	struct Query {
		/** construct a query to check if all components included.
		 * @tparam Cs... list of components which should be query for.
		 * @return Query object
		 */
		template<Component ... Cs>
		static const Query& has();

		template<template <Component...> class S, Component... Cs>
		static const Query& has(S<Cs...>) { return has<Cs...>(); }

		/** like \see has() but don't give query an id.
		 */
		template<Component ... Cs>
		static const Query& hasUnlisted();

		template<template <Component...> class S, Component... Cs>
		static const Query& hasUnlisted(S<Cs...>) { return hasUnlisted<Cs...>(); }

		/** get id from Query.
		 * Each Query has a unique id, which allows fast compare of querys for equality.
		 * @return id
		 */
		size_t id() const;

		/** get hash from query.
		 * @return hash for faster compare.
		 */
		const type_hash_t& hash() const;

		/** test if the sequence of components match the query.
		 * @tparam I iterator over type ids
		 * @param begin of sequence
		 * @param end of sequence
		 * @attention dose not test hash!
		 */
		template<QueryTidIterator I>
		bool test(I _begin, I _end) const;

		/** number required types.
		 * @return minimum length of archetype to match this query.
		 */
		size_t size() const;

	private:
		Query(size_t _id, size_t _size, const typeid_t* _tids, type_hash_t _hash);

		template<Component ... Cs>
		static Query constructQuery();

		Query registerQuery() const;

		const size_t m_id = 0; ///< unique id for each set of arguments
		const size_t m_size = 0; ///< number of components
		const typeid_t* m_tids = 0; ///< pointer to list of ids
		const type_hash_t m_hash{0}; ///< hash for faster compare with ArcheTypes

		static size_t m_count; ///< number of existing queries

		// TODO: compile time mapping between permutations
		struct Key {
			const typeid_t* data;
			size_t size;
			size_t hash;
		};
		struct Hash {
			size_t operator()(const Key& _key) const;
		};
		struct Equal {
			bool operator()(const Key& _lh, const Key& _rh) const;
		};
		using hashmap_t = std::unordered_map<Key, size_t, Hash, Equal>;

		static hashmap_t m_tidsToQid; ///< hash map to use same query id for each permutation of components
	};
} // end of namespace registry
