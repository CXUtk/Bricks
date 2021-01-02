#pragma once
#include "Scene/Scene.h"
#include "Strucures/Board.h"
#include <vector>

class DefaultScene : public Scene {
public:
    DefaultScene();
    ~DefaultScene() override;

    void update() override;
    void draw() override;

    void dfs(int x, std::vector<Brick>& bricks);
private:
    std::shared_ptr<Board> _board;

    Brick _handBrick;
    std::vector<Brick> _bricks;
};
