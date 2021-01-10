#include "InputManager.h"

InputManager::InputManager(GLFWwindow* window) :_window(window) {
    _curKeysDown = 0;
    _oldKeysDown = 0;
    _isMouseDown = false;
    _wasMouseDown = false;
}

glm::vec2 InputManager::getMousePosition() {
    double x, y;
    glfwGetCursorPos(_window, &x, &y);
    return glm::vec2(x, y);
}

bool InputManager::isKeyDown(int key) {
    return glfwGetKey(_window, key) == GLFW_PRESS;
}

void InputManager::beginInput() {
    // Input start
    _isMouseDown = isMouseLeftDown();
    _isMouseRightDown = isMouseRightDown();
    for (int i = 32; i < 200; i++) {
        _curKeysDown.set(i, isKeyDown(i));
    }
}

void InputManager::endInput() {
    _wasMouseDown = _isMouseDown;
    _wasMouseRightDown = _isMouseRightDown;
    _oldKeysDown = _curKeysDown;
}
