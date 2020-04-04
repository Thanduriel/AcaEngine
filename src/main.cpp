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
#include "game/actions/drawModels.hpp"
#include "game/actions/applyVelocity.hpp"
#include "game/actions/updateTransform.hpp"
#include "game/actions/processLifetime.hpp"
#include <spdlog/spdlog.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/color_space.hpp>
#include <glm/gtx/compatibility.hpp>

// CRT's memory leak detection
#ifndef NDEBUG 
#if defined(_MSC_VER)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#include <random>
#include <chrono>

int main()
{
#ifndef NDEBUG 
#if defined(_MSC_VER)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//	_CrtSetBreakAlloc(2760);
#endif
#endif

	using namespace glm;
	using namespace std::chrono;
	using namespace graphics;

	if (!Device::initialize(1366, 768, false)) return -1;
	GLFWwindow* window = Device::getWindow();

	{
		using namespace game;
		using namespace components;

		Registry<Model, Position, Velocity, Transform, Lifetime> registry;
		LifetimeManager manager;

		Mesh mesh(*utils::MeshLoader::get("../resources/models/crate.obj"));
		Sampler sampler(Sampler::Filter::LINEAR, Sampler::Filter::LINEAR, Sampler::Filter::LINEAR, Sampler::Border::CLAMP);
		const Texture2D& texture = *graphics::Texture2DManager::get("../resources/textures/cratetex.png", sampler);

		std::default_random_engine rng;
		std::uniform_real_distribution<float> dist;

		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		Camera camera;
		camera.projection = perspective(glm::radians(70.f), 16.f / 9.f, 0.01f, 100.f);
		camera.view = lookAt(vec3(0.f, 0.f, 10.f), vec3(0.f), vec3(0.f, 1.f, 0.f));

		Camera orthoCam;
		orthoCam.viewProjection= glm::ortho(0.0f, (float)w, 0.0f, (float)h, 0.f, 1.f);
		
		operations::DrawModels drawModels;
		drawModels.setCamera(camera);

		SpriteRenderer spriteRenderer;
		Sprite sprite(0.5f, 0.5f, &texture);
		spriteRenderer.draw(sprite, vec3(0.f, 0.f, -0.1f), 0.f, vec2(0.5f,0.5f));

		steady_clock::time_point begin = steady_clock::now();
		float spawnTime = 0.f;

		glClearColor(0.0f, 0.3f, 0.6f, 1.f);
		while (!glfwWindowShouldClose(window))
		{
			const steady_clock::time_point end = steady_clock::now();
			const duration<float> d = duration_cast<duration<float>>(end - begin);
			const float dt = d.count();
			begin = end;

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			spawnTime += dt;
			if (spawnTime >= 0.5f)
			{
				Entity ent = registry.create();
				registry.addComponent<Model>(ent, mesh, texture, glm::identity<mat4>());
				registry.addComponent<Model>(ent, mesh, texture, rotate(glm::identity<mat4>(), pi<float>() / 2.f, vec3(0.f, 0.f, 1.f)));
				registry.addComponent<Position>(ent, vec3(0.f));
				registry.addComponent<Transform>(ent, identity<mat4>());
				registry.addComponent<Velocity>(ent, vec3(dist(rng)*2.f-1.0f, dist(rng)*2.f-1.0f, 0.f));
				registry.addComponent<Lifetime>(ent, 1.f + dist(rng) * 5.f);

				spawnTime = 0.f;
			}

			registry.execute(operations::ApplyVelocity(dt));
			registry.execute(operations::UpdateTransformPosition());
			registry.execute(operations::ProcessLifetime(manager, dt));
			registry.execute(drawModels);
			drawModels.present();
			spriteRenderer.present(orthoCam);

			manager.cleanup(registry);

			glfwPollEvents();
			glfwSwapBuffers(window);
			Sleep(3);
		}
	}

	Device::close();
	utils::MeshLoader::clear();

	return 0;
}
