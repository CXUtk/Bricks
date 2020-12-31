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
    bool _wasMouseDown;
    bool _isMouseDown;

    std::bitset<256> _curKeysDown;
    std::bitset<256> _oldKeysDown;

    Brick _handBrick;
};
