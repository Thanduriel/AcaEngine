#include "testutils.hpp"
#include <vector>
#include <engine/game/core/registry.hpp>
#include <engine/game/core/registry2.hpp>
#include <engine/game/components/simpleComponents.hpp>
#include <engine/game/operations/applyVelocity.hpp>

using namespace game;
using namespace components;

struct TestComponent
{
	TestComponent(const std::string& _value) : value(_value) {}
	std::string value;
};

class TestOperation
{
public:
	void operator()(TestComponent& _label,
		const components::Position& _position,
		const components::Velocity& _velocity)
	{
		counter++;
		_label.value = std::to_string(static_cast<int>(_position.value.z + 2.f * _velocity.value.z));
	}

	int counter = 0;
};

template<typename Registry>
void testRegistry()
{
	Registry registry;

	std::vector<Entity> entities;

	entities.push_back(registry.create());
	EXPECT(entities[0], "Registry creates a valid entity.");
	registry.addComponent<components::Position>(entities[0], glm::vec3(1.f, 2.f, 3.f));
	EXPECT(registry.hasComponent<Position>(entities[0]), "Entity has added component.");
	EXPECT(registry.getComponent<Position>(entities[0]).value == glm::vec3(1.f, 2.f, 3.f),
		"Component was created correctly.");

	// not implemented!
	//registry.removeComponent<Position>(entities[0]);
	//EXPECT(!registry.hasComponent<Position>(entities[0]), "Remove component.");

	for (int i = 0; i < 13; ++i)
		entities.push_back(registry.create());
	

	for (size_t i = 1; i < 7; ++i)
	{
		registry.addComponent<Position>(entities[i], glm::vec3(static_cast<float>(i)));
		registry.addComponent<Velocity>(entities[i], glm::vec3(0.f, 0.f, static_cast<float>(i)));
	}

	for (size_t i = 1; i < 7; ++i)
	{
		EXPECT(registry.hasComponent<Position>(entities[i]), "Entities have added components.");
		EXPECT(registry.hasComponent<Velocity>(entities[i]), "Entities have added components.");
	}
	for (size_t i = 7; i < 13; ++i)
	{
		EXPECT(!registry.hasComponent<Velocity>(entities[i]), "Entities only have added components.");
		EXPECT(!registry.hasComponent<Velocity>(entities[i]), "Entities only have added components.");
	}

	registry.execute(operations::ApplyVelocity(2.f));
	for (size_t i = 1; i < 7; ++i)
	{
		EXPECT(registry.getComponent<Position>(entities[i]).value == glm::vec3(static_cast<float>(i), static_cast<float>(i), 3.f * static_cast<float>(i)),
			"Entities have added components.");
	}

	for (size_t i = 0; i < 13; i += 2)
		registry.addComponent<TestComponent>(entities[i], std::to_string(i) + "aaaaaaaa");

	Registry::EntityRef ref = registry.getRef(entities[2]);
	EXPECT(registry.getEntity(ref), "Create valid entity ref.");

	for (size_t i = 0; i < 4; ++i)
		registry.erase(entities[i]);
	entities.erase(entities.begin(), entities.begin() + 4);

	EXPECT(!registry.getEntity(ref), "Ref is no longer valid after erasing the entity.");

	TestOperation op;
	registry.execute(op);
	EXPECT(op.counter == 2, "Execute operation on fitting entities after delete.");
	EXPECT(registry.getComponent<TestComponent>(entities[0]).value == std::to_string(5 * 4),
				"Execute operation on fitting entities after delete.");

	for (int i = 0; i < 5; ++i)
	{
		Entity ent = registry.create();
		registry.addComponent<Position>(entities[i], glm::vec3(static_cast<float>(i*42)));
		registry.addComponent<Velocity>(entities[i], glm::vec3(0.f, 0.f, static_cast<float>(i*42)));
		entities.push_back(ent);
	}
	
	registry.addComponent<TestComponent>(entities.back(), "foo");
	registry.execute(op);
	EXPECT(op.counter == 5, "Execute operation on fitting entities after second create.");
}

int main()
{
	testRegistry<game::Registry2>();
	testRegistry<game::Registry<Position, Velocity, TestComponent>>();
	return 0;
}