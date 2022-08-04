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
		: m_targetFrameTime(0.f)
	{
		auto& videoConfig = utils::Config::get()["video"];
		const int width = utils::Config::getValue(videoConfig, "resolutionX", 1366);
		const int height = utils::Config::getValue(videoConfig, "resolutionY", 768);
		const bool fullScreen = utils::Config::getValue(videoConfig, "fullScreen", false);
		if (!graphics::Device::initialize(width, height, fullScreen))
		{
			spdlog::error("Could not initialize the device. Shutting down.");
			abort();
		}

		const bool vsync = utils::Config::getValue(videoConfig, "vsync", false);
		graphics::Device::setVSync(vsync);

		input::InputManager::initialize(graphics::Device::getWindow());

		const int targetFPS = utils::Config::getValue(videoConfig, "targetFPS", 60);
		m_targetFrameTime = 1.f / targetFPS;
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
			graphics::Device::setZWrite(true);
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
			input::InputManager::updateKeyStates();
			glfwPollEvents();
			if (glfwWindowShouldClose(window)) m_gameStates.clear();

			// frame rate control
			const auto currentD = duration_cast<duration<float>>(high_resolution_clock::now() - start);
			const std::chrono::duration<float> targetFrameTime(m_targetFrameTime);
			const auto remaining = targetFrameTime - currentD - 0.5ms;
			if (remaining.count() > 0.f)
			{
#ifdef WIN32
				// in windows threads yield at least for the remainder of time-slice
				// which takes ~16ms
				if (targetFrameTime > 16ms && currentD < 15.5ms)
					std::this_thread::sleep_for(1ms);
#else
				std::this_thread::sleep_for(remaining);
#endif
			}
			while ((high_resolution_clock::now() - start) < targetFrameTime) {}
		}
	}
}