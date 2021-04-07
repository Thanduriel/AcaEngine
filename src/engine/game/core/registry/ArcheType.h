/**
 * Project Untitled
 */

#pragma once

#include "Entity.h"
#include "Query.h"
#include "TypeArray.h"
#include "types.h"

#include <array>
#include <vector>

namespace registry {

	/** class to handle all data acorded to one Archetype
	 * @todo faster component loock up!
	 */
	class Archetype
	{
		Archetype();
		/** constructed Empty Archetype from component list.
		 * @param _hash of Archetype
		 * @param _comps Components of archetype
		 */
		Archetype(type_hash_t&& _hash, std::vector<TypeArray>&& _comps, size_t _capacity);
	public:
		Archetype( const Archetype& ) = delete;
		Archetype& operator=( const Archetype& ) = delete;

		Archetype( Archetype&& );
		Archetype& operator=( Archetype&& );

		/** construct a new Archetype, as extension of an the current one.
		 * @tparam Cs... component types to add
		 * @param _capacity capacity of new Archetype
		 * @return constructed Archetype
		 */
		template<Component... Cs>
		requires( sizeof...( Cs ) > 0 ) Archetype addComponents(size_t _capacity = 0);

		/** construct new Archetype, with less components then this one.
		 * @tparam Cs... component types wihch contained in this type but should not in the new type
		 * @param _capacity inital capacity of new archetype
		 * @return constructed Archetype
		 */
		template<Component ... Cs>
		requires(sizeof...(Cs) > 0) Archetype removeComponents(size_t _capacity = 0);

		/** construct Archetype with given components.
		 * @tparam Cs... component types which included in Archetype.
		 * @param _capacity initial capacity(number of elements which can be stored).
		 * @return constructed Archetype
		 */
		template<Component... Cs>
		requires( sizeof...( Cs ) > 0 ) static Archetype withComponents(size_t _capacity = 0);

		/** change capacity to required size.
		 * @param _capacity new capacity of Archetype.
		 */
		void allocate(size_t _capacity);

		/** migrates an entry from other Archetype by adding components.
		 * @tparam Cs component types to add (at least one)
		 * @param _src_type source Archetype to migrate from
		 * @param _src_id id of source entry source Archetype
		 * @param _newComponents components to add after migration (at least one)
		 * @return id of migrated entry
		 */
		template<Component... Cs>
		requires( sizeof...( Cs ) > 0 )
		  size_t migrateFrom( Archetype& _src_typ, size_t _src_id, Cs... _newComponents );

		/** migrates an entry from other Archetype by removing components.
		 * @param _src_type source Archetype
		 * @param _src_id id in source Archetype
		 * @return id of migrated entry
		 */
		size_t migrateFrom( Archetype& _src_type, size_t _src_id );

		/** test if Archetype matches query
		 * @param _query to check
		 * @return true if type matches query
		 * @return false else
		 */
		bool test( const Query& _query ) const;

		/** creates an new entry.
		 * @tparam Cs... Types of components of the entry
		 * @param _ent entity identifier for new entity
		 * @param components... instances to use for the entry
		 * @return id from created entry
		 */
		template<Component... Cs>
		size_t create( Entity _ent, Cs... _components );

		/** removes one entry.
		 * @param _id id of entry to remove
		 */
		void remove( size_t _id );

		/** duplicates a entry
		 * @param _id id of entry to duplicate
		 * @param _ent entity identifier for new entity
		 * @return id from new entry
		 * @attention all components from this entry must provide a copy function
		 */
		size_t duplicate(Entity _ent, size_t _id);

		/** execute functor on each entity
		 * @tparam Fn type of functor
		 * @param _functor to execute
		 */
		template<typename Fn>
		void execute( Fn& _functor );

		/** same as \link Archetype::execute() but for rvalues */
		template<typename Fn>
		void execute( const Fn& _functor );

		/** get component from one entity.
		 * @tparam C component type to access
		 * @param _id of entity to access
		 * @return nullptr if component not exist in this archetype
		 * @return pointer to component else
		 */
		template<Component C>
		C* getComponent( size_t _id );

		/// const version of \link getComponent
		template<Component C>
		const C* getComponent( size_t _id ) const;

		/** get entity identifier.
		 * @param _id slot of entity
		 * @return Entity identifier of entity in slot
		 */
		Entity getEntity(size_t _id) const;

		struct TypeIdIterator {
			using iterator = std::vector<TypeArray>::const_iterator;
			iterator itr;
			TypeIdIterator(iterator _itr) : itr{_itr}{}
			typeid_t operator*() const;
			bool operator==(const TypeIdIterator& _other) const;
			TypeIdIterator operator++();
			TypeIdIterator operator++(int);
		};

		/** return object to iterate over contained types in order.
		 * @return object with begin() and end() to get TypeIdIterators
		 */
		auto getTypeIds() const;

		size_t size() const;
		size_t capacity() const;
		size_t numComponents() const;
		const type_hash_t& typeHash() const;


	private:
		template<Component ... Cs>
		const std::array<size_t, sizeof...(Cs)>& getPermutation();

		template<typename Fn, Component... Cs>
		void executeUnpacked( Fn& _functor, utils::UnpackFunctor<std::remove_cv_t<Fn>, Cs...> );

		template<Component C, Component... Cs, typename Fn, Component... Itr>
		void executeHelper( Fn& _functor, Itr* ... itr );

		template<typename Fn, Component... Cs>
		void executionIteration( Fn& _functor, Cs*... cons );

		type_hash_t            m_typeHash;
		std::vector<TypeArray> m_components;
		size_t 				   m_entitySize;
		std::vector<Entity>    m_entities;
		std::vector<char>      m_data;
		size_t                 m_size;
		size_t                 m_capacity;
	};
}   // end of namespace registry
