#pragma once

#include "./ArcheType.h"

#include "./TypeArray.hpp"
#include "./Query.hpp"

#include <cassert>
#include <exception>

namespace registry {

	inline auto Archetype::getTypeIds() const
	{
		struct range {
			TypeIdIterator begin() const { return m_begin; }
			TypeIdIterator end() const { return m_end; }
		private:
			friend auto Archetype::getTypeIds() const;
			range(TypeIdIterator _begin, TypeIdIterator _end)
				: m_begin{_begin}, m_end{_end} {}
			TypeIdIterator m_begin;
			TypeIdIterator m_end;
		};
		return range(m_components.begin(), m_components.end());
	}

	inline Archetype::Archetype::Archetype()
	 : m_typeHash( 0 ),
	   m_components{},
	   m_entitySize( 0 ),
	   m_entities{},
	   m_data( 0 ),
	   m_size( 0 ),
	   m_capacity( 0 )
	{}

	inline Archetype::Archetype( type_hash_t&& _hash, std::vector<TypeArray>&& _comps, size_t _capacity )
	 : m_typeHash( _hash ),
	   m_components( _comps ),
	   m_entitySize( 0 ),
	   m_entities{},
	   m_data( 0 ),
	   m_size( 0 ),
	   m_capacity( 0 )
	{
		std::sort(
		  m_components.begin(), m_components.end(),
		  []( const TypeArray& _lh, const TypeArray& _rh ) { return _lh.type().id() < _rh.type().id(); } );

		for ( size_t i = 0; i < m_components.size(); ++i )
		{
			const TypeInfo& type = m_components[i].type();
			m_entitySize += type.size();
		m_entities.resize(_capacity);
		}

		allocate( _capacity );
	}

	inline Archetype::Archetype( Archetype&& _other )
	 : m_typeHash( std::move( _other.m_typeHash ) ),
	   m_components( std::move( _other.m_components ) ),
	   m_entities( std::move( _other.m_entities ) ),
	   m_entitySize( _other.m_entitySize ),
	   m_data( std::move( _other.m_data ) ),
	   m_size( _other.m_size ),
	   m_capacity( _other.m_capacity )
	{
		_other.m_size     = 0;
		_other.m_capacity = 0;
	}

	inline Archetype& Archetype::operator=( Archetype&& _other )
	{
		m_typeHash          = std::move( _other.m_typeHash );
		m_components        = std::move( _other.m_components );
		m_entities          = std::move( _other.m_entities );
		m_data              = std::move( _other.m_data );
		m_entitySize 		= _other.m_entitySize;
		m_size              = _other.m_size;
		_other.m_size       = 0;
		m_capacity          = _other.m_capacity;
		_other.m_capacity   = 0;
		return *this;
	}

	template<Component ... Cs>
	requires(sizeof...(Cs)>0)
	inline Archetype Archetype::addComponents(size_t _capacity)
	{
		std::vector<TypeArray> types(m_components);
		(types.emplace_back(TypeInfo::get<Cs>(), nullptr), ...);
		type_hash_t hash = (m_typeHash | ... | TypeInfo::get<Cs>().hash());
		return Archetype(std::move(hash), std::move(types), _capacity);
	}
	template<Component ... Cs>
	requires(sizeof...(Cs) > 0)
	inline Archetype Archetype::removeComponents(size_t _capacity)
	{
		const std::array<typeid_t, sizeof...(Cs)>& tsDel =
			utils::orderedTypes<Cs...>();

		std::vector<TypeArray> types{};
		type_hash_t hash{};

		auto dPtr = tsDel.cbegin();
		for(const TypeArray& ty : m_components)
		{
			const TypeInfo& type = ty.type();
			if (dPtr != tsDel.cend()
					&& *dPtr == type.id()) {
				++dPtr;
			} else {
				types.emplace_back(type, nullptr);
				hash |= type.hash();
			}
		}
		return Archetype(std::move(hash), std::move(types), _capacity);
	}

	template<Component... Cs>
	requires( sizeof...( Cs ) > 0 ) inline Archetype Archetype::withComponents( size_t _capacity )
	{
		return Archetype(
		  ( TypeInfo::get<Cs>().hash() | ... ),
		  { TypeArray::ofType<Cs>()... },
		  _capacity );
	}

	inline void Archetype::allocate( size_t _capacity )
	{
		if (_capacity == 0) { return; }
		assert( _capacity >= m_size );
		m_entities.resize(_capacity);
		std::vector<char> newData( m_entitySize * _capacity );

		char* ptr = newData.data();
		for ( TypeArray& arr : m_components )
		{
			arr.newMemory( ptr, m_size );
			ptr += _capacity * arr.type().size();
		}

		m_data     = std::move( newData );
		m_capacity = _capacity;
	}

	template<Component... Cs>
	requires( sizeof...( Cs ) > 0 ) inline size_t Archetype::migrateFrom( Archetype& _src_type, size_t _src_id, Cs... _newComponents )
	{
		// FIXME: two moves for required
		// no copy needed when all components are moved!
		// std::array<void*, sizeof...( Cs )> comps{ &_newComponents... };
		assert( m_size < m_capacity );
		const auto& permutation = getPermutation<Cs...>();
		size_t      count       = 0;

		const static std::array<size_t, sizeof...( Cs ) + 1> offsets = { 0, ( count += sizeof( Cs ) )... };

		char data[( sizeof( Cs ) + ... )];
		{
			char* ptr = data;
			( ( new (ptr) Cs(std::forward<Cs>(_newComponents)), ptr += sizeof( Cs ) ), ... );
		}

		auto                               srcPtr = _src_type.m_components.begin();
		auto                               newId  = permutation.begin();
		for ( auto dstPtr = m_components.begin(); dstPtr != m_components.end(); ++dstPtr )
		{
			if ( srcPtr != _src_type.m_components.end() && srcPtr->type() == dstPtr->type() )
			{
				dstPtr->move( *srcPtr++, _src_id, m_size );
			}
			else
			{
				dstPtr->move( data + offsets[*newId++], m_size );
			}
		}

		m_entities[m_size] = _src_type.m_entities[_src_id];
		_src_type.remove(_src_id);

		return m_size++;
	}

	inline size_t Archetype::migrateFrom( Archetype& _src_type, size_t _src_id )
	{
		assert(m_size < m_capacity);
		auto dstPtr = m_components.begin();
		auto srcPtr = _src_type.m_components.begin();
		for ( auto dstPtr = m_components.begin(); dstPtr != m_components.end(); ++dstPtr )
		{
			while ( srcPtr->type() != dstPtr->type() )
			{
				++srcPtr;
			}
			dstPtr->move( *srcPtr++, _src_id, m_size );
		}

		m_entities[m_size] = _src_type.m_entities[_src_id];
		_src_type.remove(_src_id);

		return m_size++;
	}

	inline typeid_t Archetype::TypeIdIterator::operator*() const
	{
		return itr->type().id();
	}

	inline bool Archetype::TypeIdIterator::operator==(const TypeIdIterator& _other) const
	{
		return itr == _other.itr;
	}

	inline Archetype::TypeIdIterator Archetype::TypeIdIterator::operator++()
	{
		++itr;
		return *this;
	}

	inline Archetype::TypeIdIterator Archetype::TypeIdIterator::operator++(int)
	{
		auto copy = *this;
		++itr;
		return copy;
	}

	inline bool Archetype::test( const Query& _query) const
	{
		if ((m_typeHash & _query.hash()) == _query.hash()
				&& numComponents() >= _query.size()) {
			auto range = getTypeIds();
			return _query.test<TypeIdIterator>(range.begin(), range.end());
		}
		return false;
	}

	template<Component... Cs>
	inline size_t Archetype::create( Entity _ent, Cs... _components )
	{
		const auto& permutation = getPermutation<Cs...>();
		size_t i           = 0;
		( m_components[permutation[i++]].template construct<Cs, Cs>( m_size, std::forward<Cs>( _components ) ), ... );
		m_entities[m_size] = _ent;
		return m_size++;
	}

	inline void Archetype::remove( size_t _id )
	{
		assert( _id < m_size );
		for ( TypeArray& arr : m_components )
		{
			arr.destruct( _id );
			if ( _id != m_size - 1 )
			{
				arr.move( m_size - 1, _id );
				arr.destruct(m_size - 1);
			}
		}
		if (_id != m_size - 1) {
			m_entities[_id] = m_entities[m_size-1];
		}
		m_entities[m_size - 1] = INVALID_ENTITY;
		--m_size;
	}

	inline size_t Archetype::duplicate( Entity _ent, size_t _id )
	{
		assert( _id < m_size );
		assert( m_size < m_capacity );
		for ( TypeArray& arr : m_components )
		{
			arr.copy( _id, m_size );
		}
		m_entities[m_size] = _ent;
		return m_size++;
	}

	template<Component... Cs>
	const std::array<size_t, sizeof...( Cs )>& Archetype::getPermutation()
	{
		const static std::array<size_t, sizeof...( Cs )> permutation = []() {
			std::array<TypeInfo, sizeof...( Cs )> types{ TypeInfo::get<Cs>()... };
			std::array<size_t, sizeof...( Cs )>   ids;
			for ( size_t i = 0; i < sizeof...( Cs ); ++i )
			{
				ids[i] = i;
			}
			std::sort( ids.begin(), ids.end(), [&types]( size_t _lh, size_t _rh ) -> bool {
				return types[_lh].id() <  types[_rh].id();
			} );
			return ids;
		}();
		return permutation;
	}
	template<Component C>
	inline C* Archetype::getComponent( size_t _id )
	{
		assert(_id < m_size);
		typeid_t tid = TypeInfo::get<C>().id();
		for(size_t i = 0; i < m_components.size(); ++i)
		{
			if (m_components[i].type().id() == tid)
			{
				return &m_components[i].get<C>(_id);
			}
		}
		return nullptr;
	}

	template<Component C>
	inline const C* Archetype::getComponent(size_t _id) const
	{
		return const_cast<Archetype&>(*this).getComponent<C>(_id);
	}

	inline Entity Archetype::getEntity(size_t _id) const
	{
		assert(_id < m_size);
		return m_entities[_id];
	}

	inline size_t Archetype::size() const { return m_size; }
	inline size_t Archetype::capacity() const { return m_capacity; }
	inline size_t Archetype::numComponents() const { return m_components.size(); }
	inline const type_hash_t& Archetype::typeHash() const { return m_typeHash; }

	template<typename Fn>
	inline void Archetype::execute( Fn& _functor )
	{
		executeUnpacked( _functor, utils::UnpackFunctor( &Fn::operator() ) );
	}

	template<typename Fn>
	inline void Archetype::execute( const Fn& _functor )
	{
		executeUnpacked( _functor, utils::UnpackFunctor( &Fn::operator() ) );
	}

	template<typename Fn, Component... Cs>
	inline void Archetype::executeUnpacked( Fn& _functor, utils::UnpackFunctor<std::remove_cv_t<Fn>, Cs...> )
	{
		executeHelper<Cs...>( _functor );
	}

	template<Component CO, Component... Cs, typename Fn, Component... Itr>
	inline void Archetype::executeHelper( Fn& _functor, Itr* ... itr )
	{
		using C = std::remove_cvref_t<CO>;
		if constexpr ( std::is_same_v<C, Entity> )
		{
			static_assert(!std::is_same_v<CO, Entity&>, "Only const access to Entity allowed!");
			if constexpr (sizeof...(Cs) > 0) {
				return executeHelper<Cs...>( _functor, itr..., m_entities.data() );
			} else {
				return executionIteration(_functor, itr..., m_entities.data());
			}
		}
		else
		{
			// FIXME: convert in O(|comps|) with sorting the arguments first!
			// TODO: possible to cache results?
			typeid_t tid   = TypeInfo::get<C>().id();
			auto     begin = m_components.begin();
			auto     end   = m_components.end();
			for ( auto i = begin; i != end; ++i )
			{
				if ( i->type().id() == tid )
				{
					if constexpr (sizeof...(Cs) > 0) {
						return executeHelper<Cs...>( _functor, itr..., i->data<C>() );
					} else {
						return executionIteration( _functor, itr...,  i->data<C>());
					}
				}
			}
		}
	}

	template<typename Fn, Component... Cs>
	inline void Archetype::executionIteration( Fn& _functor, Cs*... cons )
	{
		for ( size_t i = 0; i < m_size; ++i, ( ( ++cons ), ... ) )
		{
			_functor( *cons... );
		}
	}

}   // end namespace registry
