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
    int _cnt[MAX_BLOCKS];

    glm::ivec2 _puzzleBoardSize;

    // void randomGenerate();
    void generateBrickTextures();
    // void cutBoard();
    // void genBrick();
    // bool checkRemain();
    // void dfsCut(int r, int c);

    //void applySolverToBoard();

    //void dfs(int x, std::vector<Brick>& bricks);
    void solve();

    glm::ivec2 calculateBricksList(int maxWidth);

};
