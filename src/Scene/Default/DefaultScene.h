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


private:
    static constexpr int MAX_BLOCKS = 100;
    std::shared_ptr<Board> _board;

    Brick _handBrick;
    int _handBrickID;
    std::vector<Brick> _bricks;
    std::vector<std::shared_ptr<Texture2D>> _textures;
    int _cnt[MAX_BLOCKS];

    void randomGenerate();
    void generateBrickTextures();
    void cutBoard();
    void genBrick();
    bool checkRemain();
    void dfsCut(int r, int c);
    void dfs(int x, std::vector<Brick>& bricks);
};
