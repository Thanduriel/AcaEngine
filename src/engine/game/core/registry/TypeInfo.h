/**
 * Project Untitled
 */

#pragma once

#include "./types.h"

#include <compare>

namespace registry {

	class TypeInfo
	{
	public:
		TypeInfo()                  = delete;
		TypeInfo( const TypeInfo& ) = default;
		TypeInfo( TypeInfo&& )      = default;
		TypeInfo& operator=( const TypeInfo& ) = delete;
		TypeInfo& operator=( TypeInfo&& ) = delete;

		/**
		 * @return size of type in bytes.
		 */
		size_t size() const;

		/**
		 * @return id of type
		 */
		typeid_t id() const;

		auto operator<=>( const TypeInfo& ) const;
		bool operator==( const TypeInfo& ) const;

		/** call destructor for object.
		 * @param ptr valid object which should be destructed
		 */
		void remove( void* ptr ) const;

		/** move object with its move operator.
		 * @param src valid object which should move
		 * @param dst enough place to move object to
		 */
		void move( void* src, void* dst ) const;

		/** copy object with copy operator.
		 * @param src valid object which shuld copied.
		 * @param dst enough space to copy object to.
		 */
		void copy( void* src, void* dst ) const;

		/** Get type informations for given type.
		 * @tparam T type to query type info for.
		 * @return finished TypeInfo object
		 */
		template<Component T>
		static TypeInfo const& get();

		/** hash for type in type info.
		 * @return bit representing this type.
		 */
		type_hash_t hash() const;

	private:
		TypeInfo( size_t _size, typeid_t _id, type_hash_t _hash, move_fn_t _move, destructor_t _destructor, copy_fn_t _copy )
		 : m_size{ _size },
		   m_id{ _id },
		   m_hash{ _hash },
		   m_move{ _move },
		   m_destructor{ _destructor },
		   m_copy{ _copy } {}

		static typeid_t m_count;   ///< count instances of TypeInfo

		const size_t       m_size;         ///< size in bytes of type
		const typeid_t     m_id;           ///< type id
		const type_hash_t  m_hash;         ///< type hash
		const move_fn_t    m_move;         ///< move function for type
		const destructor_t m_destructor;   ///< destructor for type
		const copy_fn_t    m_copy;         ///< copy function for type
	};

	namespace utils {

		/// returns reference to ordered list of tids
		template<Component ... Cs>
		const std::array<typeid_t, sizeof...(Cs)>& orderedTypes();

	} // end namespace utils

}   // end of namespace registry
