/**
 * Project Untitled
 */

#pragma once

#include "types.h"

namespace registry {

	struct Record
	{
		constexpr Record()
		 : archetype{ static_cast<unsigned>( -1 ) },
		   id{ static_cast<unsigned>( -1 ) } {}
		bool operator==(const Record&) const = default;
	private:
		friend class ArchetypeRegistry;
		Record( unsigned _archetype, unsigned _id )
		 : archetype{ _archetype },
		   id{ _id } {}
		unsigned archetype;   ///< Archetype id
		unsigned id;          ///< id in of entity in Archetype
	};
	constexpr Record INVALID_RECORD{};

	struct Entity
	{
		constexpr Entity()
		 : id{ static_cast<unsigned>( -1 ) } {}

		Entity( const Entity& ) = default;
		Entity& operator=( const Entity& ) = default;
		Entity( Entity&& )                 = default;
		Entity& operator=( Entity&& )              = default;
		auto    operator<=>( const Entity& ) const = default;
		bool    operator!=( const Entity& ) const  = default;

	private:
		friend class ArchetypeRegistry;
		Entity( unsigned _id )
		 : id{ _id } {}
		unsigned id;
	};

	struct EntityRef
	{
	private:
		friend class ArchetypeRegistry;
		EntityRef( Entity _ent, generation_t _generation )
		 : ent{ _ent },
		   generation{ _generation } {}

		Entity       ent;
		generation_t generation;
	};

	constexpr Entity INVALID_ENTITY{};
}   // end of namespace registry
