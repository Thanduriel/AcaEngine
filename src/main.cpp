#include "graphics/renderer/fontrenderer.hpp"
#include "graphics/core/shader.hpp"
#include "graphics/core/device.hpp"
#include "graphics/renderer/mesh.hpp"
#include "graphics/renderer/meshrenderer.hpp"
#include "game/core/registry.hpp"
#include "game/actions/drawModels.hpp"
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

		Registry<Model> registry;
	/*	utils::MeshData test;
		test.positions.push_back(vec3(-1.f, -1.f, 0.f));
		test.positions.push_back(vec3(1.f, -1.f, 0.f));
		test.positions.push_back(vec3(1.f, 1.f, 0.f));
		test.positions.push_back(vec3(-1.f, 1.f, 0.f));
		test.normals.resize(4, vec3(1.f, 0.f, 0.f));
		test.textureCoordinates.resize(4, vec2(0.f));
		test.faces.push_back({});
		test.faces[0].indices[0].positionIdx = 0;
		test.faces[0].indices[1].positionIdx = 1;
		test.faces[0].indices[2].positionIdx = 2;
		test.faces.push_back({});
		test.faces[1].indices[0].positionIdx = 3;
		test.faces[1].indices[1].positionIdx = 0;
		test.faces[1].indices[2].positionIdx = 2;*/

		MeshRenderer renderer;
		actions::DrawMeshes drawMeshes;
		Mesh mesh(*utils::MeshLoader::get("../resources/models/crate.obj"));

		Entity ent = registry.create();
		registry.addComponent<Model>(ent, mesh, glm::identity<mat4>());

		int w, h;
		glfwGetFramebufferSize(window, &w, &h);
		//mat4x4 viewProj = glm::ortho(0.0f, (float)w, 0.0f, (float)h, 0.f, 1.f);
		mat4x4 viewProj = perspective(glm::radians(70.f), 16.f / 9.f, 0.01f, 100.f) * lookAt(vec3(0.f, 0.f, 10.f), vec3(0.f), vec3(0.f, 1.f, 0.f));
		drawMeshes.setViewProjection(viewProj);

		while (!glfwWindowShouldClose(window))
		{
			glClearColor(0.0f, 0.3f, 0.6f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT);

			registry.execute(drawMeshes);
			drawMeshes.present();

			glfwPollEvents();
			glfwSwapBuffers(window);
		}
	}

	Device::close();
	utils::MeshLoader::clear();

	return 0;
}
