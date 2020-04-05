#pragma once

#include "actions.hpp"
#include "keys.hpp"
#include <glm/vec2.hpp>

struct GLFWwindow;

namespace input
{

	class InputInterface
	{
	public:
		virtual bool isKeyPressed(Action _action) const = 0;
		virtual float getAxis(Axis _axis) const = 0;
		// in screen space
		virtual glm::vec2 getCursorPos() const = 0;
	};

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
}