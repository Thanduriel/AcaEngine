#include "transforms.hpp"

namespace game {
namespace systems {

	namespace comps = components;

	void Transforms::update(Components _comps)
	{
		using namespace glm;

		_comps.execute([&](const comps::Children& _childs, const comps::TransformNeedsUpdate&)
			{
				for (Entity e : _childs.entities)
					getComp<comps::TransformNeedsUpdate>(_comps).add(e);
			});

		_comps.execute([&](Entity _ent, const comps::TransformNeedsUpdate&, comps::Transform& _transform, const comps::Position& _position)
			{
				glm::mat4 transform = glm::identity<glm::mat4>();

				if (comps::Scale* scale = getComp<comps::Scale>(_comps).get(_ent))
					transform = glm::scale(transform, scale->value);
				if (comps::Rotation* rot = getComp<comps::Rotation>(_comps).get(_ent))
					transform = transform * glm::mat4(rot->value);

				transform[3] = glm::vec4(_position.value, 1.f);

				_transform.value = transform;
			});

		_comps.execute([&](Entity _ent, const comps::TransformNeedsUpdate&, const comps::Parent& _parent, comps::Transform& _transform)
			{
				if (auto parentTransform = getComp<comps::Transform>(_comps).get(_parent.entity))
				{
					_transform = parentTransform->value * _transform.value;
				}
			});

		getComp<comps::TransformNeedsUpdate>(_comps).clear();
	}
}
}