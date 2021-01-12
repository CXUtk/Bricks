#include "InputManager.h"


int scrollValue;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    scrollValue = (int)yoffset;
}

InputManager::InputManager(GLFWwindow* window) :_window(window) {
    _curKeysDown = 0;
    _oldKeysDown = 0;
    _isMouseDown = false;
    _wasMouseDown = false;

    glfwSetScrollCallback(window, scroll_callback);
}

int InputManager::getScrollValue() const {
    return scrollValue;
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
    scrollValue = 0;
}
