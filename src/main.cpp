#include "engine/graphics/renderer/fontrenderer.hpp"
#include "engine/graphics/core/shader.hpp"
#include "engine/graphics/core/device.hpp"
#include "engine/graphics/renderer/mesh.hpp"
#include "engine/graphics/renderer/meshrenderer.hpp"
#include "engine/graphics/renderer/spriterenderer.hpp"
#include "engine/graphics/camera.hpp"
#include "engine/graphics/core/texture.hpp"
#include "engine/graphics/core/sampler.hpp"
#include "engine/game/core/registry.hpp"
#include "engine/game/core/registry2.hpp"
#include "engine/game/operations/drawModels.hpp"
#include "engine/game/operations/applyVelocity.hpp"
#include "engine/game/operations/updateTransform.hpp"
#include "engine/game/operations/processLifetime.hpp"
#include "engine/game/core/game.hpp"
#include "engine/input/inputmanager.hpp"
#include "engine/utils/config.hpp"
#include "engine/input/keyboardInterface.hpp"
#include <engine/utils/typeIndex.hpp>
#include <engine/utils/containers/weakSlotMap.hpp>
#include <spdlog/spdlog.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/color_space.hpp>
#include <glm/gtx/compatibility.hpp>

#include <fstream>
#include <filesystem>
#include <iostream>
#include <random>
#include <chrono>

// CRT's memory leak detection
#ifndef NDEBUG 
#if defined(_MSC_VER)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

using namespace game;
using namespace components;
using namespace graphics;
using namespace glm;
/*
enum struct Actions {
	EXIT,
	MOVE_LEFT,
	MOVE_RIGHT
};

enum struct Axis {
	MOVE_L_R
};

class MainState : public game::GameState
{
public:
	MainState()
		: m_manager(m_registry)
	{
		m_inputs = std::unique_ptr<input::InputInterface>(
			new input::KeyboardInterface(utils::Config::get()["inputs"]["keyboard"], 
				{ {"exit", input::Key::ESCAPE}, {"moveLeft", input::Key::LEFT}, {"moveRight", input::Key::RIGHT} },
				{ {Actions::MOVE_LEFT, Actions::MOVE_RIGHT} }));
	}

	void process(float _deltaTime)
	{
		static std::default_random_engine rng;
		static std::uniform_real_distribution<float> dist;
		static Mesh mesh(*utils::MeshLoader::get("../resources/models/crate.obj"));
		static Sampler sampler(Sampler::Filter::LINEAR, Sampler::Filter::LINEAR, Sampler::Filter::LINEAR, Sampler::Border::CLAMP);
		static const Texture2D& texture = *graphics::Texture2DManager::get("../resources/textures/cratetex.png", sampler);


		static float spawnTime = 0.f;
		spawnTime += _deltaTime;
		if (spawnTime >= 0.5f)
		{
			Entity ent = m_registry.create();
			m_registry.addComponent<Model>(ent, mesh, texture, glm::identity<mat4>());
			m_registry.addComponent<Model>(ent, mesh, texture, rotate(glm::identity<mat4>(), pi<float>() / 2.f, vec3(0.f, 0.f, 1.f)));
			m_registry.addComponent<Position>(ent, vec3(0.f));
			m_registry.addComponent<Transform>(ent, identity<mat4>());
			m_registry.addComponent<Velocity>(ent, vec3(dist(rng) * 2.f - 1.0f, dist(rng) * 2.f - 1.0f, 0.f));
			m_registry.addComponent<Lifetime>(ent, 1.f + dist(rng) * 5.f);

			spawnTime = 0.f;
		}

		m_registry.execute(operations::ApplyVelocity(_deltaTime));
		m_registry.execute(operations::UpdateTransformPosition());
		m_registry.execute(operations::ProcessLifetime(m_manager, _deltaTime));

		if (m_inputs->isKeyPressed(Actions::EXIT)) finish();
	}

	void draw(float _deltaTime)
	{
		Camera camera(70.f, 0.01f, 100.f);
		camera.setView(lookAt(vec3(0.f, 0.f, 10.f), vec3(0.f), vec3(0.f, 1.f, 0.f)));

		Camera orthoCam(Device::getBufferSize());

		operations::DrawModels drawModels;
		drawModels.setCamera(camera);

	//	SpriteRenderer spriteRenderer;
	//	Sprite sprite(0.5f, 0.5f, &texture);
	//	spriteRenderer.draw(sprite, vec3(0.f, 0.f, -0.1f), 0.f, vec2(0.5f, 0.5f));

		m_registry.execute(drawModels);
		drawModels.present();
	//	spriteRenderer.present(orthoCam);
	}

private:
	Registry<Model, Position, Velocity, Transform, Lifetime> m_registry;
	LifetimeManager<Model, Position, Velocity, Transform, Lifetime> m_manager;
	std::unique_ptr<input::InputInterface> m_inputs;
};
*/

namespace chrono = std::chrono;

template<bool RequiresTy, class Constructor>
std::tuple<float,float,float> benchmarkSlotMap(Constructor constructor)
{
	constexpr int numElements = 2 << 8; //17
	constexpr int numRuns = 1024;

	float tInsert = 0.f;
	float tIterate = 0.f;
	float tRemove = 0.f;
	float lenSum = 0.f;

	for (int i = 1; i < numRuns+1; ++i)
	{
		std::default_random_engine rng(i * 13567);
		std::uniform_int_distribution<int> dist(0, numElements);
		std::vector<int> entities(numElements, 0);
		std::iota(entities.begin(), entities.end(), 0);
		std::shuffle(entities.begin(), entities.end(), rng);

		using Ty = components::Position;

		auto slotMap = constructor();

		auto start = chrono::high_resolution_clock::now();
		for (int i = 0; i < numElements; i++)
			if constexpr(RequiresTy)
				slotMap.emplace<Ty>(entities[i], glm::vec3(static_cast<float>(i)));
			else
				slotMap.emplace(entities[i], glm::vec3(static_cast<float>(i)));
		auto end = chrono::high_resolution_clock::now();
		tInsert += chrono::duration<float>(end - start).count();

		start = chrono::high_resolution_clock::now();
		glm::vec3 sum(0.f);
		if constexpr (RequiresTy)
			for (const Ty& el : slotMap.iterate<Ty>())
				sum += el.value;
		else 
			for (const Ty& el : slotMap)
				sum += el.value;
		end = chrono::high_resolution_clock::now();
		tIterate += chrono::duration<float>(end - start).count();

		start = chrono::high_resolution_clock::now();
		for (int i = 0; i < numElements; i += 2)
			slotMap.erase(entities[i]);
		end = chrono::high_resolution_clock::now();
		tRemove += chrono::duration<float>(end - start).count();

		lenSum += glm::length(sum) / 2.f;
	}

	std::cout << "insert random elements: " << tInsert / numRuns << std::endl;
	std::cout << "iterate over all elements: " << tIterate / numRuns << std::endl;
	std::cout << "remove every other element: " << tRemove / numRuns << std::endl;
	std::cout << lenSum << "\n";

	return { tInsert, tIterate, tRemove };
}

namespace game { namespace components {
	struct TestComponent
	{
		TestComponent( const std::string& _text,
			float _fontSize = 10.f,
			const glm::vec3& _pos = glm::vec3(0.f, 0.f, -0.5f),
			const Color& _color = Color(1.f),
			const Alignment& _alignment = Alignment(0.f, 0.f),
			float _rotation = 0.f,
			bool _roundToPixel = false)
			: size(_text.length()), fontSize(_fontSize), position(_pos), color(_color),
			rotation(_rotation), alignment(_alignment),
			roundToPixel(_roundToPixel)
		{}

		size_t size;
		float fontSize;
		glm::vec3 position;
		Color color;
		float rotation;
		Alignment alignment;
		bool roundToPixel;
	};
}}

class TestOperation
{
public:
	void operator()(components::TestComponent& _label,
		const components::Transform& _transform,
		const components::Position& _position,
		const components::Velocity& _velocity) const
	{
		const glm::vec4 v = _transform.value * glm::vec4(_position.value, 1.f);
		_label.size = (std::to_string(v.x) + ", " + std::to_string(v.y) + std::to_string(_velocity.value.z)).size();
	//	_label.text = std::to_string(v.x) + ", " 
	//		+ std::to_string(v.y) + std::to_string(_velocity.value.z);
	}
};

struct Results
{
	union{
		struct {
			float tInsert;
			float tIterateSimple;
			float tIterateComplex;
			float tRemove;
		};
		std::array<float, 4> values = {};
	};
};
template<typename Registry>
Results benchmarkRegistry(int numEntities, int numRuns)
{
	auto run = [&](Results& results) 
	{
		Registry registry;

		std::vector<Entity> entities;
		entities.reserve(numEntities);

		for (int i = 0; i < numEntities; ++i)
			entities.push_back(registry.create());

		registry.addComponent<components::Position2D>(entities.front(), glm::vec2(0.2f));
		registry.addComponent<components::Rotation2D>(entities.front(), 42.f);

		auto start = chrono::high_resolution_clock::now();
		for (int i = 0; i < numEntities; ++i)
		{
			registry.addComponent<components::Position>(entities[i], glm::vec3(static_cast<float>(i)));
			registry.addComponent<components::Velocity>(entities[i], glm::vec3(1.f, 0.f, static_cast<float>(i)));
		}
		auto end = chrono::high_resolution_clock::now();
		results.tInsert += chrono::duration<float>(end - start).count();

		std::default_random_engine rng(13567);
		std::shuffle(entities.begin(), entities.end(), rng);

		start = chrono::high_resolution_clock::now();
		for (int i = 0; i < numEntities; i += 7)
			registry.addComponent<components::TestComponent>(entities[i], std::to_string(i) + "2poipnrpuipo");
		for (int i = 0; i < numEntities; i += 3)
			registry.addComponent<components::Transform>(entities[i], glm::identity<glm::mat4>());
		end = chrono::high_resolution_clock::now();
		results.tInsert += chrono::duration<float>(end - start).count();

		std::uniform_real_distribution<float> dt(0.01f, 0.5f);
		start = chrono::high_resolution_clock::now();
		registry.execute(operations::ApplyVelocity(dt(rng)));
		end = chrono::high_resolution_clock::now();
		results.tIterateSimple += chrono::duration<float>(end - start).count();

		start = chrono::high_resolution_clock::now();
		registry.execute(TestOperation());
		end = chrono::high_resolution_clock::now();
		results.tIterateComplex += chrono::duration<float>(end - start).count();

		/*	int count = 0;
			registry.execute([&](const components::Label& label) {
				count += label.text.size();
			});
			std::cout << count << "\n";*/

		start = chrono::high_resolution_clock::now();
		for (int i = 0; i < numEntities; ++i)
			registry.erase(entities[i]);
		end = chrono::high_resolution_clock::now();
		results.tRemove += chrono::duration<float>(end - start).count();
	};

	Results tempResults;
	run(tempResults);
	run(tempResults);

	Results results;
	for (int j = 0; j < numRuns; ++j)
	{
		run(results);
	}

	for (float& f : results.values)
		f /= numRuns;
/*	std::cout << "add components: " << results.tInsert << std::endl;
	std::cout << "execute fast operation: " << results.tIterateSimple << std::endl;
	std::cout << "execute large operation: " << results.tIterateComplex << std::endl;
	std::cout << "erase entities: " << results.tRemove << std::endl;*/

	return results;
}

template<typename Warmup, typename... RegistryTypes>
void runComparison()
{
//	(benchmarkRegistry<RegistryTypes>(2 << 16, 8), ...);
	benchmarkRegistry<Warmup>(2 << 16, 64);

	std::vector<Results> results;
	(results.push_back(benchmarkRegistry<RegistryTypes>(2 << 16, 32)), ...);

	for (const float& f : results.front().values)
		std::cout << f << "\n";

	printf("\ninsert     simple_op  complex_op remove     \n");
	for (const auto& result : results)
	{
		for (size_t i = 0; i < result.values.size(); ++i)
		{
			printf("%.3f      ", result.values[i] / results.front().values[i]);
		}
		printf("\n");
	}

}

int main()
{
#ifndef NDEBUG 
#if defined(_MSC_VER)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	_CrtSetBreakAlloc(2760);
#endif
#endif

	
/*	auto [t3, t4, t5] = benchmarkSlotMap<true>([]() { return utils::WeakSlotMap<int>(static_cast<components::Position*>(nullptr)); });
	auto [t0, t1, t2] = benchmarkSlotMap<false>([]() { return utils::SlotMap<int, components::Position>(); });
	std::cout << "ratios: " << t0 / t3 << " | " << t1 / t4 << " | " << t2 / t5 << std::endl;
	*/

	using GameRegistry = game::Registry <
		components::Position,
		components::Velocity,
		components::Transform,
		components::Label,
		components::TestComponent,
		components::Position2D,
		components::Rotation2D>;

	runComparison<GameRegistry, GameRegistry, game::Registry2>();
//	Game game;
//	game.run(std::make_unique<MainState>());

	return 0;
}
