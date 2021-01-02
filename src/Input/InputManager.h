#pragma once
#include "Game.h"
#include <bitset>
class InputManager {
public:
    InputManager(GLFWwindow* window);

    void beginInput();
    void endInput();

    bool getOldMouseDown() const { return _wasMouseDown; }
    bool getCurMouseDown() const { return _isMouseDown; }
    bool getWasKeyDown(int key) const { return _oldKeysDown.test(key); }
    bool getIsKeyDown(int key)const { return _curKeysDown.test(key); }
    glm::vec2 getMousePosition();

private:
    bool _wasMouseDown;
    bool _isMouseDown;

    std::bitset<256> _curKeysDown;
    std::bitset<256> _oldKeysDown;
    GLFWwindow* _window;

    bool isMouseLeftDown() {
        return glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    }

    bool isKeyDown(int key);
};
