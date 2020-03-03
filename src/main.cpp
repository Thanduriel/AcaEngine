#include "graphics/renderer/fontrenderer.hpp"
#include "graphics/core/shader.hpp"
#include "graphics/core/device.hpp"
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

	graphics::FontRenderer fontRenderer;
	fontRenderer.createFont("../resources/fonts/Anonymous Pro.ttf", reinterpret_cast<const char*>(u8" 0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzäöüß#´`'\"^_@%&|,;.:!?~+-*/(){}[]<>\U000003B5\U000003A9\U0000262F\U00002713"));
//	fontRenderer.storeCaf("../resources/fonts/OpenSans.caf");
//	fontRenderer.loadCaf("../resources/fonts/OpenSans.caf");

	Program shader;
	shader.attach(ShaderManager::get("../resources/shader/font.vert", ShaderType::VERTEX));
	shader.attach(ShaderManager::get("../resources/shader/font.geom", ShaderType::GEOMETRY));
	shader.attach(ShaderManager::get("../resources/shader/font.frag", ShaderType::FRAGMENT));
	shader.link();

	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	mat4x4 viewProj = glm::ortho(0.0f, (float)w, 0.0f, (float)h, 0.f, 1.f);
	shader.setUniform(0, viewProj);

	std::default_random_engine rng;
	std::uniform_real_distribution<float> dist;

	auto getRandomColor = [&]() { return glm::rgbColor(vec3(dist(rng)* 360.f, 1.f, 1.f)); };

	vec3 iniCol = getRandomColor();
	vec3 tarCol = getRandomColor();
	float timeSum = 0.f;

	steady_clock::time_point begin = steady_clock::now();

	while (!glfwWindowShouldClose(window))
	{
		const steady_clock::time_point end = steady_clock::now();
		const duration<float> d = duration_cast<duration<float>>(end - begin);
		const float dt = d.count();
		begin = end;

		timeSum += dt * 0.5f;
		const vec3 curCol = glm::lerp(iniCol, tarCol, timeSum);
		glClearColor(curCol.r, curCol.g, curCol.b, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		if (timeSum >= 1.f)
		{
			timeSum = 0.f;
			iniCol = curCol;
			tarCol = getRandomColor();
		}
		shader.use();
		fontRenderer.clearText();
		fontRenderer.draw(vec3(250.f, 500.f, 0.f), "[](){ return \"Hello world!\"; }", 50.0f, vec4(1.f,1.f,1.f,1.f));
		fontRenderer.present();

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	Device::close();

	return 0;
}