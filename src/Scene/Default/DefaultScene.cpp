#include "DefaultScene.h"
#include "Game.h"
#include <glm/gtx/transform.hpp>
#include <algorithm>

DefaultScene::DefaultScene() {
    _board = std::make_shared<Board>(10, 10, glm::vec2(250, 250));
    _curKeysDown = 0;
    _oldKeysDown = 0;
    _isMouseDown = false;
    _wasMouseDown = false;
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

    _bricks.push_back(o1);
    _bricks.push_back(o2);
    _bricks.push_back(o3);
    _bricks.push_back(o4);
    //bricks.push_back(o4);
    //bricks.push_back(o4);
    //bricks.push_back(o5);
    //bricks.push_back(o5);
    // bricks.push_back(o5);
    dfs(0, _bricks);
    // _board->place(o1, glm::ivec2(0, 0), 1, TileType::BRICK);
    printf("%d\n", 5);
    Brick test(4, 4,
        ".OO."
        "OOOO"
        "O..O"
        "O..O");

    _handBrick = test;
}

DefaultScene::~DefaultScene() {
}

void DefaultScene::update() {
    _board->update();
    auto& game = Game::GetInstance();
    auto input = game.getInputManager();
    auto mousePos = input->getMousePosition();

    auto pos = _board->getIndexFromPos(glm::vec2(mousePos.x, game.getHeight() - mousePos.y));
    bool canplace = _board->canPlace(_handBrick, pos);
    _board->placeShadow(_handBrick, pos, canplace ? 1 : 2);


    // Input start
    _isMouseDown = input->isMouseLeftDown();
    for (int i = 32; i < 200; i++) {
        _curKeysDown.set(i, input->isKeyDown(i));
    }

    if (_isMouseDown && !_wasMouseDown && canplace) {
        _board->place(_handBrick, pos, 1, TileType::BRICK);
    }
    if (_curKeysDown.test(GLFW_KEY_Z) && !_oldKeysDown.test(GLFW_KEY_Z)) {
        _handBrick = _handBrick.rotateClockwise();
    }

    if (_curKeysDown.test(GLFW_KEY_X) && !_oldKeysDown.test(GLFW_KEY_X)) {
        _handBrick = _handBrick.flip();
    }
    // Input end
    _wasMouseDown = _isMouseDown;
    _oldKeysDown = _curKeysDown;

}

void DefaultScene::draw() {
    _board->draw();
    _board->clearShadow();

    for (int i = 0; i < _bricks.size(); i++) {
        auto texture = _bricks[i].generateTexture(glm::vec3(1, 1, 0));
        int startX = 69 * i;
        float scale = std::min(64.f / texture->getSize().x, 64.f / texture->getSize().y);
        scale = std::min(scale, 1.0f);
        Game::GetInstance().getGraphics()->drawSprite(texture, glm::vec2(startX, 0), scale, glm::vec3(1));
    }
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
    Brick b = bricks[x].flip();
    int r = _board->getRows(), c = _board->getCols();
    for (int s = 0; s < 2; s++) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j <= r - b.n; j++) {
                for (int k = 0; k <= c - b.m; k++) {
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
