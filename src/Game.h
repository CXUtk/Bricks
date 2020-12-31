#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Scene/Scene.h"
#include "Render/Shader/ShaderManager.h"
#include "Render/Graphic.h"
#include "Input/InputManager.h"
class InputManager;
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
    std::shared_ptr<InputManager> getInputManager()const {
        return _inputManager;
    }

private:
    // 构造函数private声明，确保单例
    Game(int width, int height);

    int _width, _height;
    GLFWwindow* _window;

    std::shared_ptr<Scene> _curScene;
    std::shared_ptr<ShaderManager> _shaderManager;
    std::shared_ptr<Graphics> _graphics;
    std::shared_ptr<InputManager> _inputManager;

    void update();
    void draw();
};
