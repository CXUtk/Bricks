#pragma once
#include "Game.h"
class InputManager {
public:
    InputManager(GLFWwindow* window);
    bool isMouseLeftDown() {
        return glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    }
    glm::vec2 getMousePosition();
    bool isKeyDown(int key);

private:
    bool mouseLeftDown;
    GLFWwindow* _window;
};
