#pragma once

#include "./Query.h"

#include "./TypeInfo.hpp"

#include <algorithm>

namespace registry {

	template<Component... Cs>
	inline Query Query::constructQuery()
	{
		// TODO: use getPermutation?
		static typeid_t tids[] = {
		  TypeInfo::get<Cs>().id()... };
		std::sort( tids, tids + sizeof...( Cs ) );
		type_hash_t hash = ( TypeInfo::get<Cs>().hash() | ... | type_hash_t{} );

		return Query(
		  std::numeric_limits<typeid_t>::max(),
		  sizeof...(Cs),
		  tids,
		  hash );
	}

	inline Query Query::registerQuery() const
	{
		Key key{
		  .data = m_tids,
		  .size = m_size,
		  .hash = static_cast<size_t>(std::hash<type_hash_t>{}(m_hash)) };

		hashmap_t::iterator query = m_tidsToQid.find( key );
		if ( query == m_tidsToQid.end() )
		{
			query = m_tidsToQid.insert( { key, m_count++ } ).first;
		}
		return Query (
		  query->second,
		  m_size,
		  m_tids,
		  m_hash );
	}

	template<Component... Cs>
	inline const Query& Query::has()
	{
		static Query query = hasUnlisted<Cs...>().registerQuery();
		return query;
	}

	template<Component... Cs>
	inline const Query& Query::hasUnlisted()
	{
		static Query query = constructQuery<Cs...>();
		return query;
	}

	inline size_t Query::id() const
	{
		return m_id;
	}

	inline size_t Query::size() const
	{
		return m_size;
	}

	inline const type_hash_t& Query::hash() const
	{
		return m_hash;
	}

	template<QueryTidIterator I>
	inline bool Query::test(I _begin, I _end) const
	{
		I comps = _begin;
		for(const typeid_t* i = m_tids; i != m_tids + m_size; ++i)
		{
			while(comps != _end && *comps < *i) { ++comps; }
			if (comps == _end || *comps++ != *i) { return false; }
		}
		return true;
	}

	inline Query::Query( size_t _id, size_t _size, const typeid_t* _tids, type_hash_t _hash )
	 : m_id{ _id },
	   m_size{ _size },
	   m_tids{ _tids },
	   m_hash{ _hash } {}

	inline size_t Query::Hash::operator()(const Query::Key& _key) const
	{
		return _key.hash;
	}

	inline bool Query::Equal::operator()(const Key& _lh, const Key& _rh) const
	{
		if (_lh.size != _rh.size) { return false; }
		for(size_t i = 0; i < _lh.size; ++i)
		{
			if (_lh.data[i] != _rh.data[i])
			{
				return false;
			}
		}
		return true;
	}

}   // end namespace registry
