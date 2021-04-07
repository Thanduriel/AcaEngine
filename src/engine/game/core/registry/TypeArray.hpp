#pragma once

#include "./TypeArray.h"

#include "./TypeInfo.hpp"

#include <cassert>

namespace registry {

	inline TypeArray::TypeArray( const TypeInfo& _info, void* _data )
	 : m_info{ _info },
	   m_data{ static_cast<char*>( _data ) } {}

	inline TypeArray& TypeArray::operator=(TypeArray&& _other)
	{
		new (&m_info) TypeInfo(std::move(_other.m_info));
		m_data = _other.m_data;
		// _other.m_data = nullptr; // TODO: gain?
		return *this;
	}

	inline TypeArray& TypeArray::operator=(const TypeArray& _other)
	{
		new (&m_info) TypeInfo(_other.m_info);
		m_data = _other.m_data;
		return *this;
	}

	template<Component T>
	inline TypeArray TypeArray::ofType( void* _data )
	{
		return TypeArray( TypeInfo::get<T>(), _data );
	}

	inline const TypeInfo& TypeArray::type() const
	{
		return m_info;
	}

	inline void TypeArray::newMemory( void* _data, size_t _old_size )
	{
		if (m_data) {
			char* src = static_cast<char*>( m_data );
			char* dst = static_cast<char*>( _data );
			for ( size_t i = 0; i < _old_size * m_info.size(); i += m_info.size() )
			{
				m_info.move( src + i, dst + i );
				m_info.remove(src + i);
			}
		}
		m_data = static_cast<char*>( _data );
	}

	template<Component T, typename... Args>
	inline void TypeArray::construct( size_t id, Args... args )
	{
		assert( m_data );
		new ( reinterpret_cast<T*>( m_data ) + id ) T( std::forward<Args>( args )... );
	}

	inline void TypeArray::destruct( size_t _id )
	{
		m_info.remove( m_data + _id * m_info.size() );
	}

	inline void TypeArray::move( size_t _src, size_t _dst )
	{
		m_info.move( m_data + _src * m_info.size(), m_data + _dst * m_info.size() );
	}

	inline void TypeArray::move( TypeArray& _src_arr, size_t _src_id, size_t _dst )
	{
		assert( m_info == _src_arr.m_info );   // Moving only allowed for same type arrays
		m_info.move( _src_arr.m_data + _src_id * m_info.size(), m_data + _dst * m_info.size() );
	}

	inline void TypeArray::move(void* _src, size_t _dst)
	{
		m_info.move(_src, m_data + _dst * m_info.size());
	}

	inline void TypeArray::copy(size_t _src, size_t _dst)
	{
		m_info.copy(m_data + _src * m_info.size(), m_data + _dst * m_info.size());
	}

	template<typename T> requires Component<T> || std::is_same_v<T,void>
	inline T* TypeArray::data()
	{
		assert( m_data );
		return reinterpret_cast<T*>( m_data );
	}

	template<Component T>
	inline T& TypeArray::get( size_t id )
	{
		assert( m_data );
		return reinterpret_cast<T*>( m_data )[id];
	}

	template<Component T>
	inline const T& TypeArray::get( size_t id ) const
	{
		assert( m_data );
		return reinterpret_cast<const T*>( m_data )[id];
	}

}   // end namespace registry
