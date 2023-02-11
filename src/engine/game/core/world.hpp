#pragma once

#include "registry2.hpp"
#include "lifetimeManager2.hpp"
#include <static_type_info.h>

namespace game {

	namespace details {
		using TypeIndex = std::remove_const_t<static_type_info::TypeIndex>;
		enum struct AccessReq : char { Read, Write };
		using RequirementsInfo = utils::HashMap<TypeIndex, AccessReq>;

		// Generic lookup in the resource tuple.
		// Handles both const and mutable reference types 
		// since the unqualified reference is returned.
		template<typename Resource>
		struct ResourceFetch
		{
			ResourceFetch() = delete; // mark for ensureResource

			using ResourceType = std::remove_cvref_t<Resource>;

			template<typename World>
			static ResourceType& get(World& world)
			{
				if constexpr (utils::contains_type_v<ResourceType, decltype(world.m_resources)>)
					return std::get<ResourceType>(world.m_resources);
				else
				{
					auto it = world.m_dynamicResources.find(static_type_info::getTypeIndex<ResourceType>());
					ASSERT(!!it, "Requested resource needs to exist.");
					return *reinterpret_cast<ResourceType*>(it.data().get());
				}
			}

			static void registerRequirements(RequirementsInfo& reqs)
			{
				constexpr TypeIndex typeIndex = static_type_info::getTypeIndex<ResourceType>();
				reqs.add(typeIndex, std::is_const_v<Resource> ? AccessReq::Read : AccessReq::Write);
			}
		};


		// game time
		template<>
		struct ResourceFetch<float>
		{
			template<typename World>
			static float get(World& world)
			{
				return world.m_deltaTime;
			}
			static void registerRequirements(RequirementsInfo& reqs)
			{
				constexpr TypeIndex typeIndex = static_type_info::getTypeIndex<float>();
				reqs.add(typeIndex, AccessReq::Read);
			}
		};

		template<>
		struct ResourceFetch<LifetimeManager2&>
		{
			template<typename World>
			static LifetimeManager2& get(World& world)
			{
				return world.m_manager;
			}

			static void registerRequirements(RequirementsInfo& reqs)
			{
				constexpr TypeIndex typeIndex = static_type_info::getTypeIndex<LifetimeManager2>();
				reqs.add(typeIndex, AccessReq::Write);
			}
		};

		// Read component
		template<typename Comp>
		struct ResourceFetch <ReadAccess<Comp>>
		{
			template<typename World>
			static ReadAccess<Comp> get(World& world)
			{
				return ReadAccess<Comp>(world.m_registry.template getContainer<Comp>());
			}

			static void registerRequirements(RequirementsInfo& reqs)
			{
				constexpr TypeIndex typeIndex = static_type_info::getTypeIndex<Comp>();
				reqs.add(typeIndex, AccessReq::Read);
			}
		};

		// Write component
		template<typename Comp>
		struct ResourceFetch <WriteAccess<Comp>>
		{
			template<typename World>
			static WriteAccess<Comp> get(World& world)
			{
				return WriteAccess<Comp>(world.m_registry.template getContainer<Comp>());
			}

			static void registerRequirements(RequirementsInfo& reqs)
			{
				constexpr TypeIndex typeIndex = static_type_info::getTypeIndex<Comp>();
				reqs.add(typeIndex, AccessReq::Write);
			}
		};

		// Component Tuple
		template<typename... CompAccess>
		struct ResourceFetch <ComponentTuple<CompAccess...>>
		{
			template<typename World>
			static ComponentTuple<CompAccess...> get(World& world)
			{
				return ComponentTuple<CompAccess...>(ResourceFetch<CompAccess>::get(world)...);
			}

			static void registerRequirements(RequirementsInfo& reqs)
			{
				(ResourceFetch<CompAccess>::registerRequirements(reqs), ...);
			}
		};
	}

	template<typename T>
	concept system_type = requires { &T::update; };

	enum struct SystemGroup 
	{
		Process,
		Draw,
		COUNT
	};

	template<typename... Resources>
	class World
	{
	public:
		World(Resources&&... _resources) 
			: m_deltaTime(0.f),
			m_manager(m_registry),
			m_resources(EntityCreator(m_registry), EntityDeleter(), std::forward<Resources>(_resources)...)
		{}

		void process(SystemGroup _group, float _deltaTime)
		{
			m_deltaTime = _deltaTime;
			auto& systemGroup = m_systems[static_cast<size_t>(_group)];
			for (auto& sysHolder : systemGroup)
				sysHolder.runFn(*this, sysHolder.system.get());
		}

		void cleanup()
		{
			m_manager.moveComponents();
			m_manager.cleanup();
			std::get<EntityDeleter>(m_resources).cleanup(m_registry);
		}

		// direct resource access; should not be used
		Registry2& getRegistry() { return m_registry; }
		// only for debugging
		LifetimeManager2& getManager() { return m_manager; }
		// currently only for static resources
		template<typename Resource>
		Resource& getResource() { return std::get<Resource>(m_resources); }

		// Create a dynamic resource explicitly.
		// If a resource is default constructible it will also be initialized 
		// automatically when a system requesting it is registered.
		// @param _args Arguments forwarded to the constructor of a Resource.
		template<typename Resource, typename... Args>
		Resource& addResource(Args&&... _args)
		{
			Resource* r = new Resource{ std::forward<Args>(_args)... };
			m_dynamicResources.add(static_type_info::getTypeIndex<Resource>(),
				UniquePtr(r,&destroyObject<Resource>));
			return *r;
		}

		template<system_type System, system_type... Dependencies>
		System* addSystem(std::unique_ptr<System> _system, SystemGroup _group, 
			const Dependencies*... _dependencies)
		{
			auto& systemGroup = m_systems[static_cast<size_t>(_group)];
			System* system = _system.release();

			systemGroup.emplace_back(UniquePtr(system, &destroyObject<System>),
				&runSystem<System>,
				std::vector<const void*>{static_cast<const void*>(_dependencies)...},
				scanRequirements(*system, utils::UnpackFunction(&System::update)));
			return system;
		}

		// Call member function of a system providing the expected resources.
		template<typename System, typename... ResourcesReq>
		void call(System& _system, void(System::* _func)(ResourcesReq ...)) { (_system.*_func)(details::ResourceFetch<ResourcesReq>::get(*this)...); }
		template<typename System, typename... ResourcesReq>
		void call(const System& _system, void(System::* _func)(ResourcesReq ...) const) { (_system.*_func)(details::ResourceFetch<ResourcesReq>::get(*this)...); }

		template<typename System>
		void run(System& _system) { call(_system, &System::update); }
		// This explicit version is only needed to capture rvalues.
		template<typename System>
		void run(const System& _system) { call(_system, &System::update); }
	private:
		using TypeIndex = std::remove_const_t<static_type_info::TypeIndex>;
		using AccessReq = details::AccessReq;
		using RequirementsInfo = details::RequirementsInfo;

		template<typename T>
		friend struct details::ResourceFetch;

		// Default construct a required resource if it does not exist yet.
		template<typename Res>
		void ensureResource()
		{
			using ResourceT = std::remove_cvref_t<Res>;
			// check that it is a dynamic resource which can be default constructed
			if constexpr (!std::is_default_constructible_v<details::ResourceFetch<Res>>
				&& !utils::contains_type_v<ResourceT, decltype(m_resources)>
				&& std::is_default_constructible_v<ResourceT>)
			{
				constexpr TypeIndex typeId = static_type_info::getTypeIndex<ResourceT>();
				auto it = m_dynamicResources.find(typeId);
				if (it == m_dynamicResources.end())
					addResource<ResourceT>();
			}
		}

		template<typename System, typename... ResourcesReq>
		RequirementsInfo scanRequirements(System& _system, utils::UnpackFunction<System,ResourcesReq...>)
		{
			(ensureResource<ResourcesReq>(), ...);

			// fetch once to ensure that the containers exist
			// todo: improve component container prefetching
			(details::ResourceFetch<ResourcesReq>::get(*this), ...);

			RequirementsInfo requirements;
			(details::ResourceFetch<ResourcesReq>::registerRequirements(requirements), ...);
			return requirements;
		}

		using RunSystem = void(*)(World&, void*);
		template<typename System>
		static void runSystem(World& _world, void* _sys)
		{
			System& sys = *reinterpret_cast<System*>(_sys);
			_world.call(sys, &System::update);
		}

		using DestroyObject = void(*)(void*);
		template<typename T>
		static void destroyObject(void* _obj)
		{
			T* obj = reinterpret_cast<T*>(_obj);
			delete obj;
		}

		float m_deltaTime;
		Registry2 m_registry;
		LifetimeManager2 m_manager; // legacy all in one interface

		std::tuple<EntityCreator, EntityDeleter, Resources...> m_resources;
		using UniquePtr = std::unique_ptr<void, DestroyObject>;
		utils::HashMap<TypeIndex, UniquePtr> m_dynamicResources;
		
		struct SystemHolder
		{
			UniquePtr system;
			RunSystem runFn;
			std::vector<const void*> dependencies;
			RequirementsInfo requirements;

			// Determines whether two systems can be run in parallel without race conditions.
			bool isCompatible(const SystemHolder& _other) const
			{
				for (auto& [typeIndex, req] : requirements)
				{
					auto it = _other.requirements.find(typeIndex);
					if (it != _other.requirements.end()
						&& (req == AccessReq::Write || it.data() == AccessReq::Write))
						return false;
				}

				return true;
			}
		};

		std::array< std::vector<SystemHolder>, static_cast<size_t>(SystemGroup::COUNT)> m_systems;
	};
}