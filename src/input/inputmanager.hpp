#pragma once

#include "action.hpp"
#include "keys.hpp"
#include <glm/vec2.hpp>

struct GLFWwindow;

namespace input
{
	// Simple wrapper to handle for glfw inputs.
	class InputManager
	{
	public:
		static void initialize(GLFWwindow* _window);

		static bool isKeyPressed(Key _key);
		static bool isButtonPressed(MouseButton _button);

		static glm::vec2 getCursorPos();
	private:
		static GLFWwindow* s_window;
	};

	// Interface to map game actions to keys.
	class InputInterface
	{
	public:
		virtual ~InputInterface() {}
		virtual bool isKeyPressed(Action _action) const = 0;
		virtual float getAxis(Axis _axis) const = 0;
		virtual glm::vec2 getCursorPos() const = 0;
	};
}