#include <spdlog/spdlog.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtx/color_space.hpp>
#include <glm/gtx/compatibility.hpp>

#include <random>
#include <chrono>

int main()
{
	using namespace glm;
	using namespace std::chrono;

	spdlog::info("Creating OpenGL context.");
	if (!glfwInit())
	{
		spdlog::error("Could not initialize glfw.");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

	GLFWwindow* window = glfwCreateWindow(1366, 768, "hello world", NULL, NULL);
	if (!window)
	{
		spdlog::error("Could not create window.");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	GLenum GlewInitResult = glewInit();
	if (GLEW_OK != GlewInitResult)
	{
		spdlog::error("Could not initialize glew.");
		glfwTerminate();
		return -1;
	}

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
		if (timeSum >= 1.f)
		{
			timeSum = 0.f;
			iniCol = curCol;
			tarCol = getRandomColor();
		}

		glfwPollEvents();
		glfwSwapBuffers(window);

		glClear(GL_COLOR_BUFFER_BIT);
	}

	spdlog::info("Shutting down.");
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}