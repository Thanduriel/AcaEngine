#include "inputmanager.hpp"
#include <GLFW/glfw3.h>

namespace input {

	GLFWwindow* InputManager::s_window = nullptr;

	void InputManager::initialize(GLFWwindow* _window)
	{
		s_window = _window;
	}

	bool InputManager::isKeyPressed(Key _key)
	{
		return glfwGetKey(s_window, static_cast<int>(_key)) == GLFW_PRESS;
	}

	bool InputManager::isButtonPressed(MouseButton _key)
	{
		return glfwGetMouseButton(s_window, static_cast<int>(_key)) == GLFW_PRESS;
	}

	glm::vec2 InputManager::getCursorPos()
	{
		double x, y;
		glfwGetCursorPos(s_window, &x, &y);

		return { x,y };
	}
}