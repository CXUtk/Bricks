#include "Game.h"
#include <iostream>


static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


Game::Game(int width, int height) : _width(width), _height(height) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(width, height, "Bricks!", nullptr, nullptr);
    if (!_window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to load glad!" << std::endl;
        glfwTerminate();
        return;
    }
    glfwSetFramebufferSizeCallback(_window, framebuffer_size_callback);
    glEnable(GL_LINE_SMOOTH);

}

Game& Game::GetInstance() {
    static Game game(500, 600);
    return game;
}

Game::~Game() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}

void Game::init() {
    _curScene = Scene::makeScene("DEFAULT");
    _shaderManager = std::make_shared<ShaderManager>();
    _graphics = std::make_shared<Graphics>();
    _inputManager = std::make_shared<InputManager>(_window);

}



static double oldTime;
static double oldTime2;
static int cnt;
void Game::run() {
    oldTime = oldTime2 = glfwGetTime();
    cnt = 0;
    while (!glfwWindowShouldClose(_window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        update();
        draw();

        glfwSwapBuffers(_window);
        while (glfwGetTime() - oldTime < 1.0 / 60.0) {
            glfwPollEvents();
        }
        double curTime = glfwGetTime();
        oldTime = curTime;
        if (curTime - oldTime2 > 1.0) {
            printf("FPS: %d\n", cnt);
            cnt = 0;
            oldTime2 = curTime;
        }
        cnt++;
    }
}

void Game::update() {
    _curScene->update();
}

void Game::draw() {
    _curScene->draw();
}
