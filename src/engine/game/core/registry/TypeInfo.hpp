#pragma once

#include "./TypeInfo.h"

#include <cassert>

namespace registry {
	inline auto TypeInfo::operator<=>( const TypeInfo& _other ) const
	{
		return m_id <=> _other.m_id;
	}

	inline bool TypeInfo::operator==(const TypeInfo& _other) const
	{
		return m_id == _other.m_id;
	}

	inline size_t TypeInfo::size() const
	{
		return m_size;
	}

	inline typeid_t TypeInfo::id() const
	{
		return m_id;
	}

	inline type_hash_t TypeInfo::hash() const
	{
		return m_hash;
	}

	inline void TypeInfo::move( void* src, void* dst ) const
	{
		m_move( src, dst );
	}

	inline void TypeInfo::remove( void* ptr ) const
	{
		m_destructor( ptr );
	}

	inline void TypeInfo::copy(void* src, void* dst) const
	{
		assert(m_copy);
		m_copy(src, dst);
	}

	template<Component T>
	inline const TypeInfo& TypeInfo::get()
	{
		using C = std::remove_cv_t<T>;
		if constexpr (std::is_copy_constructible_v<T>) {
			static TypeInfo typeInfo(
			  sizeof(C),
			  m_count,
			  type_hash_t{}.set( m_count % HASH_SIZE ),
			  []( void* _src, void* _dst ) { new ( _dst ) C( std::move(*static_cast<T*>( _src )) ); },
			  []( void* _obj ) {
				  static_cast<C*>( _obj )->~C();
			  },
			  [](void* _src, void* _dst) { new (_dst)C(*static_cast<const T*>(_src));}
			);
			++m_count;
			return typeInfo;
		} else {
			static TypeInfo typeInfo(
			  sizeof(C),
			  m_count,
			  type_hash_t{}.set( m_count % HASH_SIZE ),
			  []( void* _src, void* _dst ) { new ( _dst ) C( std::move(*static_cast<T*>( _src )) ); },
			  []( void* _obj ) {
				  static_cast<C*>( _obj )->~C();
			  },
			  nullptr
			);
			++m_count;
			return typeInfo;
		}
	}

	namespace utils {

		template<Component ... Cs>
		inline const std::array<typeid_t, sizeof...(Cs)>& orderedTypes()
		{
			static std::array<typeid_t, sizeof...(Cs)> sorted = [](){
				std::array<typeid_t, sizeof...(Cs)> tids = {TypeInfo::get<Cs>().id()...};
				std::sort(tids.begin(), tids.end());
				return tids;
			}();
			return sorted;
		}
	} // end namespace utils
}   // namespace registry
