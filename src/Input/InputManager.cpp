#include "InputManager.h"

InputManager::InputManager(GLFWwindow* window) :_window(window) {

}

glm::vec2 InputManager::getMousePosition() {
    double x, y;
    glfwGetCursorPos(_window, &x, &y);
    return glm::vec2(x, Game::GetInstance().getHeight() - y);
}

bool InputManager::isKeyDown(int key) {
    return glfwGetKey(_window, key) == GLFW_PRESS;
}
