#pragma once

#include <memory>

namespace game {

	class GameState
	{
	public:
		virtual ~GameState() {}
		// standard frame updates
		virtual void process(float _deltaTime) = 0;
		virtual void draw(float _deltaTime) = 0;

		// Called whenever this state becomes the active state, either on creation or
		// when the state above gets poped.
		virtual void onActivate() {}

		bool isFinished() const { return m_isFinished; }
		// Terminate this state after the current frame.
		void finish() { m_isFinished = true; }

		// Introduce a new state which will become active in the next frame.
		void setNewState(GameState& _state) { m_newState.reset(&_state); }
		std::unique_ptr<GameState> fetchNewState();
	private:
		bool m_isFinished = false;
		std::unique_ptr<GameState> m_newState;
	};
}