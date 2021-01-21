#pragma once
#include "Scene/Scene.h"
#include "Strucures/Board.h"
#include "Strucures/Puzzle.h"
#include <vector>
#include <thread>
#include <mutex>
#include <map>

class DefaultScene : public Scene {
public:
    DefaultScene();
    ~DefaultScene() override;

    void update() override;
    void draw() override;


private:
    static constexpr int MAX_BLOCKS = 1024;
    std::shared_ptr<Board> _board;
    std::shared_ptr<Puzzle> _puzzle;

    Shape _handBrick;
    int _handBrickID;
    std::vector<Shape> _shapes;
    std::vector<std::shared_ptr<Texture2D>> _textures;

    glm::ivec2 _puzzleBoardSize;
    bool _isFinished;

    // void randomGenerate();
    void generateBrickTextures();

    glm::ivec2 calculateBricksList(int maxWidth);

};
