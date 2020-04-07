#include "gamestate.hpp"

namespace game {

	std::unique_ptr<GameState> GameState::fetchNewState()
	{
		return std::move(m_newState);
	}
}