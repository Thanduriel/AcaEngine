/**
 * Project Untitled
 */


#pragma once

#include "ArcheType.h"
#include "Entity.h"
#include "Query.h"

#include <optional>

namespace registry {

	class ArchetypeRegistry {
		static constexpr size_t DEFAULT_CAPACITY = 10; ///< number of entities to allocate when constructing a new Archetype
		static constexpr float CAPACITY_GROWTH = 1.5f; ///< factor to increas archetype capacity when it is full
	public:

		/** create a new entity with given components
		 * @see createFromEntity to avoid Archetype search
		 * @tparam Cs... Component types for the entity
		 * @param _cs... components to initialise the entity
		 * @return handle to new entity
		 */
		template<Component ... Cs>
		Entity create(Cs ... _cs);

		/** creates a new entity with given components
		 * in the same Archetype then of an other entity.
		 * @tparam Cs... component types to overwrite of base entity
		 * 	when |Cs| = 0 then it will create a duplication!
		 * @param _ent entity as base
		 * @param _cs... values for component to overwrite
		 * @return handle to new entity
		 * @todo implement construction with copy
		 * @attention all not overwritten components must be copy constructable
		 */
		template<Component ... Cs>
		Entity createFromEntity(Entity _ent, Cs ... _cs);


		/** convert an entity handle to an entity refernce.
		 * A refernce allow checking if the refernced entity still the same.
		 * @param _ent handle to get refercnce to
		 * @return refernce for handle
		 */
		EntityRef getRef(Entity _ent) const;

		/** convert refernce to handle
		 * @param _ref refernce to convert
		 * @return nullopt if entity not exist anymore
		 * @return handle if entity still exist
		 */
		std::optional<Entity> getEntity(EntityRef _ref) const;


		/** add one component to an entity.
		 * @see addComponents if you want to add multiple components
		 * @tparam C component type
		 * @tparam Args... types to construct the component
		 * @param _ent entity to add components to
		 * @param _args arguments to construct components
		 * @todo handling existing component collision
		 * @attention undefined behavior for already existing components
		 */
		template<Component C, typename ... Args>
		void addComponent(Entity _ent, Args ... _args);

		/** add multiple components to one entity.
		 * Avoid archetype search, construction and moves
		 * @tparam Cs... component types to add
		 * @param _ent entity to add components to
		 * @param _cs... components to add
		 * @todo handling existing component collision
		 * @attention undefined behavior for already existing components
		 */
		template<Component ... Cs> requires (sizeof...(Cs) > 0)
		void addComponents(Entity _ent, Cs ... _cs);

		/** access component of entity.
		 * @tparam C component type to access
		 * @param _ent entity to access component from
		 * @return nullptr if entity don't have this component
		 * @return pointer to compnent if component exist
		 */
		template<Component C>
		C* getComponent(Entity _ent);

		/// const version of \link getComponent
		template<Component C >
		const C* getComponent(Entity _ent) const;

		/** just calls \link getComponent and fails miserable if component not exist.
		 * @todo can we get a performance gain from this?
		 */
		template<Component C>
		C& getComponentUnsafe(Entity _ent);

		/// const version of \link getComponentUnsafe
		template<Component C>
		const C& getComponentUnsafe(Entity _ent);

		/** removes components from entity.
		 * @tparam Cs... component types to remove
		 * @param _ent entity to remove components from
		 */
		template<Component ... Cs>
		void removeComponents(Entity _ent);

		/** remove/deletes a entity
		 * @param _ent entity to remove
		 */
		void remove(Entity _ent);
		/** alias for \link remove
		 */
		void erase(Entity _ent) { remove(_ent); }

		/** execute functor an all matching entities.
		 * Each entity which contains enough components to call the functor are used.
		 * Only exact type matches for deducing are used.
		 * @tparam F type of functor
		 * @param _functor to execute
		 */
		template<typename F >
		void execute(F _functor);

	private:
		template<Component C>
		struct is_no_entity : std::integral_constant<bool,!std::is_same_v<Entity,C>> {};

		auto archetypesWithLevel(size_t _lvl);
		static void resizeForPush(Archetype&);

		template<Component ... Cs>
		Entity createEntity(size_t _aId, Cs ..._cs);

		template<Component ... Cs>
		Record migrateEntity( Record _src, size_t _dst, Cs... _cs);

		size_t insertNewArchetype(Archetype&& _at);

		template<typename F, Component ... Args>
		size_t queryForFunctor(utils::UnpackFunctor<F, Args...>);

		void matchArchetypes(size_t _qid);
		void matchQueries(size_t _aid);

		std::vector<Record> m_records; ///< maps entity.id to record
		std::vector<generation_t> m_generations; ///< generation count for entities
		std::vector<size_t> m_emptySpots; ///< list of Entities which are not bound yet

		std::vector<Archetype>  m_archetypes; ///< list of all Archetypes
		// TODO: more place efficient?
		struct QueryResult {
			const Query* query = nullptr;
			std::vector<size_t> aIds{};
		};
		std::vector<QueryResult> m_queryResults; ///< maps query id to matching Archetypes
		// TODO: better access pattern!
		std::vector<size_t> m_archetypesByLvl{0}; ///< maps number of components to Archetypes
	};
	} // end of namespace registry
