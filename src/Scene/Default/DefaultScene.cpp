#include "DefaultScene.h"
#include "Game.h"
#include "ImUI/ImUI.h"
#include <glm/gtx/transform.hpp>
#include <algorithm>

static glm::vec3 colors[] = {
    glm::vec3(1, 0, 1),
    glm::vec3(1, 0, 0),
    glm::vec3(1, 0.8, 0),
    glm::vec3(0, 1, 0),
    glm::vec3(0, 0.9, 0),
    glm::vec3(0, 0.8, 0),
    glm::vec3(0, 1, 1),
    glm::vec3(0, 0.9, 0.9),
    glm::vec3(0, 0.8, 0.8),
};


static int selectedColor = 1;

DefaultScene::DefaultScene() {
    _board = std::make_shared<Board>(10, 10, glm::vec2(250, 250));
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
    mousePos.y = game.getHeight() - mousePos.y;

    auto pos = _board->getIndexFromPos(_handBrick, mousePos);
    bool canplace = _board->canPlace(_handBrick, pos);
    _board->placeShadow(_handBrick, pos, canplace ? 1 : 2);


    input->beginInput();
    if (input->getCurMouseDown() && !input->getOldMouseDown() && canplace && _board->mouseInside(mousePos)) {
        _board->place(_handBrick, pos, selectedColor, TileType::BRICK);
    }
    if (input->getIsKeyDown(GLFW_KEY_Z) && !input->getWasKeyDown(GLFW_KEY_Z)) {
        _handBrick = _handBrick.rotateClockwise();
    }

    if (input->getIsKeyDown(GLFW_KEY_X) && !input->getWasKeyDown(GLFW_KEY_X)) {
        _handBrick = _handBrick.flip();
    }

}

void DefaultScene::draw() {
    auto& game = Game::GetInstance();
    auto input = game.getInputManager();

    _board->draw();
    _board->clearShadow();


    // Gap: 64 + 10
    for (int i = 0; i < _bricks.size(); i++) {
        auto texture = _bricks[i].generateTexture(glm::vec3(1, 1, 0));
        int startX = 74 * i;
        float scale = std::min(64.f / texture->getSize().x, 64.f / texture->getSize().y);
        scale = std::min(scale, 1.0f);
        if (ImUI::GetInstance().button(texture, glm::vec2(startX, 0), scale, glm::vec3(1))) {
            printf("%d is Clicked!\n", i);
            _handBrick = _bricks[i];
            selectedColor = i;
        }
    }
    //std::vector<glm::vec2> lines;
    //lines.push_back(glm::vec2(0, 0));
    //lines.push_back(glm::vec2(500, 500));
    //Game::GetInstance().getGraphics()->drawLines(lines, glm::vec3(1, 1, 1), 2);


     // Input end
    input->endInput();
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
