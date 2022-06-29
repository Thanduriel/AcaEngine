#include "game.hpp"
#include "../../graphics/core/device.hpp"
#include "../../graphics/renderer/fontrenderer.hpp"
#include "../../input/inputmanager.hpp"
#include "../../utils/meshLoader.hpp"
#include "../../utils/config.hpp"
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

namespace game{

	Game::Game()
	{
		if (!graphics::Device::initialize(1366, 768, false))
		{
			spdlog::error("Could not initialize the device. Shutting down.");
			abort();
		}
		input::InputManager::initialize(graphics::Device::getWindow());
	}

	Game::~Game()
	{
		graphics::Device::close();
		utils::MeshLoader::clear();
		utils::Config::save();
	}

	void Game::run(std::unique_ptr<GameState> _initialState)
	{
		using namespace std::chrono;
		using namespace std::chrono_literals;

		m_gameStates.emplace_back(std::move(_initialState));

		GLFWwindow* window = graphics::Device::getWindow();
		auto start = high_resolution_clock::now();

		while (!m_gameStates.empty())
		{
			const auto end = high_resolution_clock::now();
			const duration<float> d = duration_cast<duration<float>>(end - start);
			const float dt = d.count();
			start = end;

			GameState& current = *m_gameStates.back();
			current.process(dt);

			// draw step
			// todo: move this into the device ?
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			current.draw(dt);
			glfwSwapBuffers(window);

			// state management with a stack
			bool changedState = false;
			std::unique_ptr<GameState> newState = current.fetchNewState();
			// check older states to prevent another frame of them being rendered
			while (m_gameStates.size() && m_gameStates.back()->isFinished()) 
			{
				m_gameStates.pop_back(); 
				changedState = true;
			}
			if (newState) 
			{
				m_gameStates.emplace_back(std::move(newState)); 
				changedState = true;
			}
			if (changedState && !m_gameStates.empty()) m_gameStates.back()->onActivate();

			// input handling
		//	input::InputManager::updateKeyStates();
			glfwPollEvents();
			if (glfwWindowShouldClose(window)) m_gameStates.clear();

			// frame rate control
			// todo: make part of config
			constexpr auto targetFrameTime = duration_cast < duration<float>>(1.666666667e+7ns);
			const auto rest = targetFrameTime - d - 0.5ms;
			if (rest.count() > 0.f)
				std::this_thread::sleep_for(rest);
			while ((high_resolution_clock::now() - start) < targetFrameTime) {}
		}
	}
}