#include "DefaultScene.h"
#include "Game.h"
#include <glm/gtx/transform.hpp>

DefaultScene::DefaultScene() {
    _board = std::make_shared<Board>(10, 10);
    Brick o1(3, 5,
        "...OO"
        ".O.OO"
        "OOOO.");
    Brick o2(4, 4,
        ".O.."
        "OOO."
        ".OOO"
        "..O.");
    Brick o3(5, 6,
        "O....."
        "O....."
        "OO...."
        ".OOOOO"
        "....O.");
    Brick o4(4, 5,
        "...O."
        "..OOO"
        ".OOO."
        "OO...");
    Brick o5(5, 5,
        "OO..."
        ".O..."
        ".O..."
        ".OO.."
        "..OOO");
    std::vector<Brick> bricks;
    bricks.push_back(o1);
    bricks.push_back(o2);
    bricks.push_back(o3);
    bricks.push_back(o4);
    bricks.push_back(o4);
    bricks.push_back(o4);
    bricks.push_back(o5);
    bricks.push_back(o5);
    //bricks.push_back(o5);
    dfs(0, bricks);
    // _board->place(o1, glm::ivec2(0, 0), 1, TileType::BRICK);
    printf("%d\n", 5);
}

DefaultScene::~DefaultScene() {
}

void DefaultScene::update() {
    _board->update();
    auto& game = Game::GetInstance();
    auto mousePos = Game::GetInstance().getMousePos();
    Brick test(4, 4,
        ".OO."
        "OOOO"
        "O..O"
        "O..O");


    auto pos = _board->getIndexFromPos(glm::vec2(mousePos.x, game.getHeight() - mousePos.y));
    _board->placeShadow(test, pos);
}

void DefaultScene::draw() {
    _board->draw();
    _board->clearShadow();
    //std::vector<glm::vec2> lines;
    //lines.push_back(glm::vec2(0, 0));
    //lines.push_back(glm::vec2(500, 500));
    //Game::GetInstance().getGraphics()->drawLines(lines, glm::vec3(1, 1, 1), 2);
}

bool found = false;
void DefaultScene::dfs(int x, std::vector<Brick>& bricks) {
    if (x == bricks.size()) {
        found = true;
        return;
    }
    Brick b = bricks[x];
    int r = _board->getRows(), c = _board->getCols();
    for (int s = 0; s < 2; s++) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < r; j++) {
                for (int k = 0; k < c; k++) {
                    auto coord = glm::ivec2(j, k);
                    if (_board->canPlace(b, coord)) {
                        _board->place(b, coord, x, TileType::BRICK);
                        dfs(x + 1, bricks);
                        if (found)return;
                        _board->remove(b, coord);
                    }
                }
            }
            b = b.rotateClockwise();
        }
        b = b.flip();
    }

}
