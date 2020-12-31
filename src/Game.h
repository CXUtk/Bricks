#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Scene/Scene.h"
#include "Render/Shader/ShaderManager.h"
#include "Render/Graphic.h"

class Game {
public:
    static Game& GetInstance();
    ~Game();
    void init();
    void run();

    std::shared_ptr<ShaderManager> getShaderManager()const { return _shaderManager; }
    std::shared_ptr<Graphics> getGraphics()const { return _graphics; }
    int getWidth() const { return _width; }
    int getHeight() const { return _height; }

    glm::vec2 getMousePos()const {
        double x, y;
        glfwGetCursorPos(_window, &x, &y);
        return glm::vec2(x, _height - y);
    }

private:
    // 构造函数private声明，确保单例
    Game(int width, int height);

    int _width, _height;
    GLFWwindow* _window;

    std::shared_ptr<Scene> _curScene;
    std::shared_ptr<ShaderManager> _shaderManager;
    std::shared_ptr<Graphics> _graphics;

    void update();
    void draw();
};
