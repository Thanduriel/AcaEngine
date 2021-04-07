/**
 * Project Untitled
 */

#pragma once

#include "./TypeInfo.h"

namespace registry {

	/**
	 * @brief class which wraps element handling.
	 * It's handles element transferee, creating and removing at a given range of memory.
	 *
	 * @attention expect handled memory! No free!
	 */
	class TypeArray
	{
	public:
		/** construct typed wrapper for memory.
		 * @param _info type to handle
		 * @param _data memory segment to store data.
		 */
		TypeArray( const TypeInfo& _info, void* _data );

		TypeArray(TypeArray&&) = default;
		TypeArray& operator=(TypeArray&&);

		TypeArray(const TypeArray&) = default;
		TypeArray& operator=(const TypeArray&);

		/** construct a TypeArray for given type.
		 * @tparam T type to put in array
		 * @param _data memory segment to store data.
		 */
		template<Component T>
		static TypeArray ofType( void* _data  = nullptr);

		/** received new memory.
		 * Copies all data to new memory, after this the old memory is not longer needed.
		 * \attention old memory must be valid still valid when calling this function!
		 * @param _data new start of memory
		 * @param _old_size number of elements to copy from old memory
		 */
		void newMemory( void* _data, size_t _old_size );

		/** type info for type managed by type array.
		 * @return TypeInfo of managed type
		 */
		const TypeInfo& type() const;

		/** constructs a new instance of the type in-place.
		 * @tparam T same type as type info is from.
		 * @tparam Args... types of arguments to construct T
		 * @param id position in array to construct instance
		 * @param args... arguments to construct T
		 */
		template<Component T, typename... Args>
		void construct( size_t id, Args... args );

		/** removes/deconstruct a instance.
		 * @param _id of instance to remove.
		 */
		void destruct( size_t _id );

		/** moves instance at id _src to instance at id _dst
		 * @param _src id of source instance
		 * @param _dst id of destination instance
		 */
		void move( size_t _src, size_t _dst );

		/** move element from other TypeArray to this.
		 * @param _src_arr TypeArray to move
		 * @param _src_id id of entry to move
		 * @param _dst id to insert element in this Array
		 */
		void move(TypeArray& _src_arr, size_t _src_id, size_t _dst);

		/** move element to this array
		 * @param _src pointer to element
		 * @param _dst id to insert element
		 */
		void move(void* _src, size_t _dst);

		/** duplicate entry in array.
		 * @param _src id of source
		 * @param _dst id of destination.
		 */
		void copy(size_t _src, size_t _dst);

		/** access data.
		 * @return pointer to begin of data segment
		 */
		template<typename T = void> requires Component<T> || std::is_same_v<T,void>
		T* data();

		/** access instance.
		 * @param _id of instance to access
		 * @return reference to instance
		 */
		template<Component T>
		T& get( size_t id );

		/** access instance.
		 * @param _id of instance to access
		 * @return reference to instance
		 */
		template<Component T>
		const T& get( size_t id ) const;

	private:
		TypeInfo 	   m_info;
		char*          m_data;
	};
}   // end of namespace registry
