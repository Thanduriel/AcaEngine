#pragma once

#include "gamestate.hpp"
#include <vector>

namespace game {

	// The core game class which handles states and the game loop.
	class Game
	{
	public:
		Game();
		~Game();

		// Start the game loop with _initialState.
		void run(std::unique_ptr<GameState> _initialState);

	private:
		float m_targetFrameTime;
		std::vector<std::unique_ptr<GameState>> m_gameStates;
	};
}