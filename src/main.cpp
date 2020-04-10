#include "graphics/renderer/fontrenderer.hpp"
#include "graphics/core/shader.hpp"
#include "graphics/core/device.hpp"
#include "graphics/renderer/mesh.hpp"
#include "graphics/renderer/meshrenderer.hpp"
#include "graphics/renderer/spriterenderer.hpp"
#include "graphics/camera.hpp"
#include "graphics/core/texture.hpp"
#include "graphics/core/sampler.hpp"
#include "game/core/registry.hpp"
#include "game/operations/drawModels.hpp"
#include "game/operations/applyVelocity.hpp"
#include "game/operations/updateTransform.hpp"
#include "game/operations/processLifetime.hpp"
#include "game/core/game.hpp"
#include "input/inputmanager.hpp"
#include "utils/config.hpp"
#include "input/keyboardInterface.hpp"
#include <spdlog/spdlog.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/color_space.hpp>
#include <glm/gtx/compatibility.hpp>
#include <fstream>
#include <filesystem>

// CRT's memory leak detection
#ifndef NDEBUG 
#if defined(_MSC_VER)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#include <random>
#include <chrono>

using namespace game;
using namespace components;
using namespace graphics;
using namespace glm;

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
		int w, h;
		glfwGetFramebufferSize(Device::getWindow(), &w, &h);
		Camera camera;
		camera.projection = perspective(glm::radians(70.f), 16.f / 9.f, 0.01f, 100.f);
		camera.view = lookAt(vec3(0.f, 0.f, 10.f), vec3(0.f), vec3(0.f, 1.f, 0.f));

		Camera orthoCam;
		orthoCam.viewProjection = glm::ortho(0.0f, (float)w, 0.0f, (float)h, 0.f, 1.f);

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
	LifetimeManager m_manager;
	std::unique_ptr<input::InputInterface> m_inputs;
};

int main()
{
#ifndef NDEBUG 
#if defined(_MSC_VER)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	_CrtSetBreakAlloc(2760);
#endif
#endif

	Game game;
	game.run(std::make_unique<MainState>());

	return 0;
}
