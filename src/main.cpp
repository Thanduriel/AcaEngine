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
#include "engine/game/operations/drawModels.hpp"
#include "engine/game/operations/applyVelocity.hpp"
#include "engine/game/operations/updateTransform.hpp"
#include "engine/game/operations/processLifetime.hpp"
#include "engine/game/core/game.hpp"
#include "engine/input/inputmanager.hpp"
#include "engine/utils/config.hpp"
#include "engine/input/keyboardInterface.hpp"
#include <engine/utils/typeIndex.hpp>
#include <engine/utils/containers/slotmap2.hpp>
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
int main()
{
#ifndef NDEBUG 
#if defined(_MSC_VER)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	_CrtSetBreakAlloc(2760);
#endif
#endif
	using Ty = components::Position;
	utils::WeakSlotMap<int> slotMap(static_cast<Ty*>(nullptr));
	for(int i = 0; i < 10; i+= 2)
		slotMap.emplace<Ty>(i, glm::vec3(1.f));

	slotMap.erase(4);
	slotMap.erase(8);

//	Game game;
//	game.run(std::make_unique<MainState>());

	return 0;
}
