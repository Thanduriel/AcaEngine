#pragma once

#include "gamestate.hpp"
#include <vector>

namespace game {

	class Game
	{
	public:
		Game();
		~Game();

		void run(std::unique_ptr<GameState> _initialState);

	private:
		std::vector<std::unique_ptr<GameState>> m_gameStates;
	};
}