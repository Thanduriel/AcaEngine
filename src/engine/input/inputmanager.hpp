#pragma once

#include "action.hpp"
#include "keys.hpp"
#include <glm/vec2.hpp>

struct GLFWwindow;

namespace input
{
	enum struct ActionState : char {
		UP = 0,
		PRESSED,
		RELEASED,
		DOWN
	};

	// Simple wrapper to handle glfw inputs.
	class InputManager
	{
	public:
		static void initialize(GLFWwindow* _window);

		static bool isKeyPressed(Key _key);
		static const char* getKeyName(Key _key);
		static ActionState getKeyState(Key _key);

		// mouse
		static bool isButtonPressed(MouseButton _button);
		static ActionState getButtonState(MouseButton _button);
		static glm::vec2 getCursorPos();
		enum struct CursorMode {
			NORMAL,
			HIDDEN,
			DISABLED
		};
		static void setCursorMode(CursorMode _mode);
		static glm::vec2 getScroll();

		static void updateKeyStates();
	private:
		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void buttonCallback(GLFWwindow* window, int button, int action, int mods);
		static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

		static GLFWwindow* s_window;
		static std::array<ActionState, static_cast<size_t>(Key::Count)> m_keyStates;
		static std::array<ActionState, static_cast<size_t>(Key::Count)> m_buttonStates;
		static double s_totalScrollX;
		static double s_totalScrollY;
	};

	// Interface to map game actions to keys.
	class InputInterface
	{
	public:
		virtual ~InputInterface() {}
		virtual bool isKeyPressed(Action _action) const = 0;
		virtual ActionState getKeyState(Action _action) const = 0;
		virtual float getAxis(Axis _axis) const = 0;
		virtual glm::vec2 getCursorPos() const = 0;
	};
}