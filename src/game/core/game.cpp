#include "game.hpp"
#include "graphics/core/device.hpp"
#include "input/inputmanager.hpp"
#include "utils/meshLoader.hpp"
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

namespace game{

	Game::Game()
	{
		if (!graphics::Device::initialize(1366, 768, false)) throw 42;
		input::InputManager::initialize(graphics::Device::getWindow());
	}

	Game::~Game()
	{
		graphics::Device::close();
		utils::MeshLoader::clear();
	}

	void Game::run(std::unique_ptr<GameState> _initialState)
	{
		using namespace std::chrono;
		using namespace std::chrono_literals;

		m_gameStates.emplace_back(std::move(_initialState));


		GLFWwindow* window = graphics::Device::getWindow();
		auto start = high_resolution_clock::now();
		glClearColor(0.0f, 0.3f, 0.6f, 1.f);
		while (!m_gameStates.empty())
		{
			const auto end = high_resolution_clock::now();
			const duration<float> d = duration_cast<duration<float>>(end - start);
			const float dt = d.count();
			start = end;

			GameState& current = *m_gameStates.back();
			current.process(dt);

			// draw step
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			current.draw(dt);
			glfwSwapBuffers(window);

			// state managment with a stack
			bool changedState = false;
			std::unique_ptr<GameState> newState = current.fetchNewState();
			// check older states to prevent another frame of them being rendered
			while (m_gameStates.size() && m_gameStates.back()->isFinished()) {
				m_gameStates.pop_back(); changedState = true;
			}
			if (newState) {
				m_gameStates.emplace_back(std::move(newState)); changedState = true;
			}
			if (changedState && !m_gameStates.empty()) m_gameStates.back()->onActivate();
			glfwPollEvents();
			if (glfwWindowShouldClose(window)) m_gameStates.clear();

			std::this_thread::sleep_for(3ms);
		}
	}
}