#pragma once

#include "./ArcheTypeRegistry.h"

#include "./TypeInfo.hpp"
#include "./ArcheType.hpp"

namespace registry {

	inline auto ArchetypeRegistry::archetypesWithLevel( size_t _lvl )
	{
		// TODO: don't hide this?
		struct range
		{
			struct iterator {
				iterator operator++(){ ++m_i; return *this; }
				iterator operator++(int){ return iterator(m_i++); }
				bool operator==(const iterator& _o) const { return _o.m_i == m_i; }
				size_t operator*() const { return m_i; }
			private:
				friend struct range;
				iterator(size_t _i) : m_i{_i}{}
				size_t m_i;
			};
			range( size_t _begin, size_t _end )
			 : m_begin( _begin ),
			   m_end( _end ) {}

			iterator begin() { return m_begin; }
			iterator end() { return m_end; }
			iterator m_begin;
			iterator m_end;
		};
		if (_lvl + 1 < m_archetypesByLvl.size()) {
			return range( m_archetypesByLvl[_lvl], m_archetypesByLvl[_lvl + 1] );
		} else {
			return range(0,0);
		}
	}

	template<Component... Cs>
	inline Entity ArchetypeRegistry::createEntity( size_t _aId, Cs... _cs )
	{
		Entity ent;
		if ( !m_emptySpots.empty() )
		{
			ent = m_emptySpots.back();
			m_emptySpots.pop_back();
		}
		else
		{
			ent = m_records.size();
			m_generations.push_back( 0 );
		}

		Archetype& at = m_archetypes[_aId];
		if ( at.size() == at.capacity() )
		{
			at.allocate( at.capacity() * CAPACITY_GROWTH );
		}

		m_records.push_back(Record(_aId,m_archetypes[_aId].create<Cs...>( ent, std::forward<Cs>( _cs )... ) ) );

		return ent;
	}
	template<>
	inline Entity ArchetypeRegistry::create()
	{
		Entity ent;
		if (!m_emptySpots.empty()) {
			ent = m_emptySpots.back();
			m_emptySpots.pop_back();
		}
		else 
		{
			ent = m_records.size();
			m_generations.push_back(0);
		}
		m_records.push_back(Record(-1, 0));
		return ent;
	}

	inline size_t ArchetypeRegistry::insertNewArchetype( Archetype&& _at )
	{
		size_t lvl = _at.numComponents();
		if (lvl + 2 > m_archetypesByLvl.size()) {
			m_archetypesByLvl.resize(lvl + 2, m_archetypesByLvl.back());
		}
		auto   levelEnd = m_archetypesByLvl.begin() + lvl + 1;
		size_t aid      = *levelEnd;
		auto   pos      = m_archetypes.cbegin() + aid;
		m_archetypes.insert( pos, std::forward<Archetype&&>( _at ) );
		for(auto a = m_archetypes.begin() + *levelEnd; a < m_archetypes.end(); ++a) {
			for(size_t i = 0; i < a->size(); ++i) {
				m_records[a->getEntity(i).id].archetype += 1;
			}
		}
		for ( ; levelEnd != m_archetypesByLvl.end(); ++levelEnd )
		{
			*levelEnd += 1;
		}
		matchQueries( aid );
		return aid;
	}

	template<Component... Cs>
	inline Entity ArchetypeRegistry::create( Cs... _cs )
	{
		const Query& match = Query::hasUnlisted<Cs...>();
		for ( size_t i : archetypesWithLevel( sizeof...( Cs ) ) )
		{
			if ( m_archetypes[i].test( match ) )
			{
				return createEntity( i, std::forward<Cs>( _cs )... );
			}
		}
		return createEntity(
		  insertNewArchetype( Archetype::withComponents<Cs...>( DEFAULT_CAPACITY ) ),
		  std::forward<Cs>( _cs )... );
	}

	template<Component... Cs>
	inline Entity ArchetypeRegistry::createFromEntity( Entity _ent, Cs... _cs )
	{
		return createEntity( m_records[_ent.id].archetype, std::forward<Cs>( _cs )... );
	}

	inline EntityRef ArchetypeRegistry::getRef( Entity _ent ) const
	{
		return EntityRef(_ent, m_generations[_ent.id] );
	}

	inline std::optional<Entity> ArchetypeRegistry::getEntity( EntityRef _ref ) const
	{
		if ( m_generations[_ref.ent.id] == _ref.generation )
		{
			return { _ref.ent };
		}
		else
		{
			return std::nullopt;
		}
	}

	template<Component C, typename... Args>
	inline void ArchetypeRegistry::addComponent( Entity _ent, Args... _args )
	{
		addComponents<C>( _ent, C( _args... ) );
	}
	void ArchetypeRegistry::resizeForPush(Archetype& at)
	{
		if ( at.size() == at.capacity() )
		{
			at.allocate( at.capacity() * CAPACITY_GROWTH );
		}
	}

	template<Component... Cs>
	requires( sizeof...( Cs ) > 0 ) inline void ArchetypeRegistry::addComponents( Entity _ent, Cs... _cs )
	{
		// TODO: make static copy?
		const std::array<size_t, sizeof...(Cs)>& tids = utils::orderedTypes<Cs...>();
		static type_hash_t newHashPart =
		  ( TypeInfo::get<Cs>().hash() | ... );
		Record& record = m_records[_ent.id];
		assert( record != INVALID_RECORD );
		if (record.archetype == static_cast<size_t>(-1) && record.id == 0) {
			const Query& match = Query::hasUnlisted<Cs...>();
			for (size_t i : archetypesWithLevel(sizeof...(Cs)))
			{
				if (m_archetypes[i].test(match)) 
				{
					Archetype& at = m_archetypes[i];
					resizeForPush(at);
					record.archetype = i;
					record.id = at.create<Cs...>(_ent, std::forward<Cs>(_cs)...);
					return; 
				}
			}
			auto aid = insertNewArchetype(Archetype::withComponents<Cs...>(DEFAULT_CAPACITY));
			Archetype& at = m_archetypes[aid];
			if ( at.size() == at.capacity() )
			{
				at.allocate( at.capacity() * CAPACITY_GROWTH );
			}
			record.archetype = aid;
			record.id = at.template create<Cs...>(_ent, std::forward<Cs>(_cs)...);
			return;
		}
		Archetype& at = m_archetypes[record.archetype];

		auto        tRange = at.getTypeIds();
		type_hash_t hash   = at.typeHash() | newHashPart;

		// search for matching existing archetype
		for ( size_t i : archetypesWithLevel( at.numComponents() + sizeof...(Cs)) )
		{
			Archetype& nAt = m_archetypes[i];
			if ( nAt.typeHash() == hash )
			{
				bool match   = true;
				auto tItrOld = tRange.begin();
				auto tItrNew = tids.begin();

				for ( size_t tid : nAt.getTypeIds() )
				{
					if ( tItrOld != tRange.end() && *tItrOld == tid )
					{
						++tItrOld;
					}
					else if ( tItrNew != tids.end() && *tItrNew == tid )
					{
						++tItrNew;
					}
					else
					{
						match = false;
						break;
					}
				}
				if ( match )
				{
					record = migrateEntity(record, i, std::forward<Cs>(_cs)...);
					return;
				}
			}
		}

		// create new Archetype
		size_t aid = insertNewArchetype(at.addComponents<Cs...>(DEFAULT_CAPACITY));
		if (aid <= record.archetype) { ++record.archetype; }
		record = migrateEntity(record, aid, std::forward<Cs>(_cs)...);
	}

	template<Component ... Cs>
	inline void ArchetypeRegistry::removeComponents(Entity _ent)
	{
		// TODO: look at addComponents
		const std::array<size_t, sizeof...(Cs)>& tids = utils::orderedTypes<Cs...>();

		Record& record = m_records[_ent.id];
		assert(record != INVALID_RECORD);
		Archetype& at = m_archetypes[record.archetype];

		auto tRange = at.getTypeIds();

		for (size_t i : archetypesWithLevel(at.numComponents() - sizeof...(Cs)))
		{
			Archetype& nAt = m_archetypes[i];
			// TODO: calculate actual hash for new type?
			if ((nAt.typeHash() & at.typeHash()) == nAt.typeHash())
			{
				bool match = true;
				auto tItrOld = tRange.begin();
				auto tItrDel = tids.begin();
				for(size_t tid : nAt.getTypeIds())
				{
					while(*tItrOld == *tItrDel)
					{
						++tItrOld;
						++tItrDel;
					}
					if (tItrOld != tRange.end() && *tItrOld == tid)
					{
						++tItrOld;
					}
					else
					{
						match = false;
						break;
					}

				}
				if (match)
				{
					record = migrateEntity(record, i);
					return;
				}
			}
		}
		size_t aid = insertNewArchetype(at.removeComponents<Cs...>(DEFAULT_CAPACITY));
		if (aid <= record.archetype)  { ++record.archetype; }
		record = migrateEntity(record, aid);
	}

	template<Component ... Cs>
	inline Record ArchetypeRegistry::migrateEntity(Record _src, size_t _dst, Cs... _cs)
	{
		resizeForPush(m_archetypes[_dst]);
		Record newRecord(
			_dst,
			m_archetypes[_dst].migrateFrom(
					m_archetypes[_src.archetype],
					_src.id,
					std::forward<Cs>(_cs)...)
		);
		// if the src entry was not the last in list
		if (_src.id != m_archetypes[_src.archetype].size())
		{
			m_records[m_archetypes[_src.archetype].getEntity(_src.id).id].id = _src.id;
		}
		return newRecord;
	}

	template<Component C>
	C* ArchetypeRegistry::getComponent(Entity _ent)
	{
		const Record& record = m_records[_ent.id];
		assert( record != INVALID_RECORD );
		return m_archetypes[record.archetype].getComponent<C>( record.id );
	}

	template<Component C >
	const C* ArchetypeRegistry::getComponent(Entity _ent) const
	{
		const Record& record = m_records[_ent.id];
		assert( record != INVALID_RECORD );
		return m_archetypes[record.archetype].getComponent<C>( record.id );
	}

	template<Component C>
	C& ArchetypeRegistry::getComponentUnsafe(Entity _ent)
	{
		return *getComponent<C>();
	}

	template<Component C>
	const C& ArchetypeRegistry::getComponentUnsafe(Entity _ent)
	{
		return *getComponent<C>();
	}

	inline void ArchetypeRegistry::remove( Entity _ent )
	{
		++m_generations[_ent.id];
		const Record& record = m_records[_ent.id];
		Archetype& at = m_archetypes[record.archetype];
		at.remove( record.id );
		// update record
		if (at.size() != record.id)
		{
			m_records[at.getEntity(record.id).id].id = record.id;
		}
		m_emptySpots.push_back(_ent.id);
#ifndef NDEBUG
		m_records[_ent.id] = INVALID_RECORD;
#endif
	}

	template<typename F, Component... Args>
	inline size_t ArchetypeRegistry::queryForFunctor( utils::UnpackFunctor<F, Args...> )
	{
		const Query& query = Query::has(typename utils::filter<is_no_entity, utils::sequence<std::remove_cvref_t<Args>...>>::type{});
		if ( m_queryResults.size() <= query.id() )
		{
			m_queryResults.resize( query.id() + 1 );
		}
		else if ( m_queryResults[query.id()].query )
		{
			return query.id();
		}
		m_queryResults[query.id()].query = &query;
		matchArchetypes( query.id() );
		return query.id();
	}

	// TODO: special case for empty query?
	inline void ArchetypeRegistry::matchArchetypes( size_t _qid )
	{
		QueryResult& result = m_queryResults[_qid];
		const Query& query = *result.query;
		for ( size_t i = m_archetypesByLvl[query.size()];
		      i < m_archetypes.size();
		      ++i )
		{
			if(m_archetypes[i].test(query)) {
				result.aIds.push_back(i);
			}
		}
	}

	inline void ArchetypeRegistry::matchQueries( size_t _aid )
	{
		const Archetype& at = m_archetypes[_aid];
		for(QueryResult& result : m_queryResults)
		{
			if(at.test(*result.query))
			{
				result.aIds.push_back(_aid);
			}
		}
	}

	template<typename F>
	inline void ArchetypeRegistry::execute( F _functor )
	{
		const size_t qid = queryForFunctor<F>(utils::UnpackFunctor(&F::operator()));
		for ( size_t i : m_queryResults[qid].aIds )
		{
			m_archetypes[i].execute( std::forward<F>( _functor ) );
		}
	}

}   // end namespace registry
