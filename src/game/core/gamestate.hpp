#pragma once

#include <memory>

namespace game {

	class GameState
	{
	public:
		virtual ~GameState() {}
		virtual void process(float _deltaTime) = 0;
		virtual void draw(float _deltaTime) = 0;

		virtual void onActivate() {}

		bool isFinished() const { return m_isFinished; }
		void finish() { m_isFinished = true; }
		void setNewState(GameState& _state) { m_newState.reset(&_state); }
		std::unique_ptr<GameState> fetchNewState();
	private:
		bool m_isFinished = false;
		std::unique_ptr<GameState> m_newState;
	};
}