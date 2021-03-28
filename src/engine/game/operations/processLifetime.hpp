#pragma once

#include "../core/entity.hpp"
#include "../core/lifetimeManager2.hpp"
#include "../components/simpleComponents.hpp"
//#include "../../graphics/renderer/meshrenderer.hpp"
#include <engine/game/core/componentaccess.hpp>

namespace game {
namespace systems {

	class ProcessLifetime
	{
	public:

		using Components = ComponentTuple<WriteAccess<components::Lifetime>>;

		void update(float _deltaTime, EntityDeleter& _deleter, Components _comps) const;
	};
}}
