#include "DefaultScene.h"
#include "Game.h"
#include "ImUI/ImUI.h"
#include "Utils.h"
#include <glm/gtx/transform.hpp>

#include <random>
#include <queue>
#include <string>
#include <vector>
#include <algorithm>


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
    randomGenerate();
    //_bricks.push_back(o1);
    //_bricks.push_back(o2);
    //_bricks.push_back(o3);
    //_bricks.push_back(o4);
    ////bricks.push_back(o4);
    ////bricks.push_back(o4);
    ////bricks.push_back(o5);
    //_bricks.push_back(o5);
    //// bricks.push_back(o5);
    for (int i = 0; i < _bricks.size(); i++) {
        _cnt[i] = 1;
    }
    //dfs(0, _bricks);
    // _board->place(o1, glm::ivec2(0, 0), 1, TileType::BRICK);
    printf("%d\n", 5);
    Brick test(4, 4,
        ".OO."
        "OOOO"
        "O..O"
        "O..O");

    _handBrick = test;
    _handBrickID = -1;
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
    bool canplace = (_handBrickID != -1 && _board->canPlace(_handBrick, pos) && _cnt[_handBrickID]);
    if (_handBrickID != -1)
        _board->placeShadow(_handBrick, pos, canplace ? 1 : 2);


    input->beginInput();
    if (input->getCurMouseDown() && !input->getOldMouseDown() && canplace && _board->mouseInside(mousePos)) {
        _board->place(_handBrick, pos, _handBrickID, TileType::BRICK);
        _cnt[_handBrickID]--;
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
    int startX = 32;
    int startY = 200;
    for (int i = 0; i < _bricks.size(); i++) {
        auto texture = _bricks[i].generateTexture(glm::vec3(1, 1, 1));
        float scale = std::min(64.f / texture->getSize().x, 64.f / texture->getSize().y);
        scale = std::min(scale, 1.0f);
        if (ImUI::GetInstance().button(texture, glm::vec2(startX, startY), scale, _cnt[i] > 0 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0))) {
            printf("%d is Clicked!\n", i);
            _handBrick = _bricks[i];
            _handBrickID = i;
        }
        startX += 74;
        if (startX + 64 > game.getWidth()) {
            startX = 32;
            startY -= 74;
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


struct Node {
    int r, c;
    int priority;
    Node(int r, int c, int p) :r(r), c(c), priority(p) {

    }
    bool operator<(const Node& node)const {
        return priority < node.priority;
    }
};

static int dr[4] = { 1, -1, 0, 0 };
static int dc[4] = { 0, 0, 1, -1 };
// 0 代表空格子，1代表当前没被切割的格子，2代表当前被切割的格子
static int vis[Board::MAX_BOARD_SIZE][Board::MAX_BOARD_SIZE];
static std::mt19937 mt;

void DefaultScene::randomGenerate() {
    mt.seed(19284321);
    for (int i = 0; i < Board::MAX_BOARD_SIZE; i++) {
        for (int j = 0; j < Board::MAX_BOARD_SIZE; j++) {
            vis[i][j] = 1;
        }
    }

    cutBoard();

}

void DefaultScene::cutBoard() {
    for (int i = 0; i < 4; i++) {
        int r, c;
        do {
            r = mt() % Board::MAX_BOARD_SIZE;
            c = mt() % Board::MAX_BOARD_SIZE;

        } while (vis[r][c] != 1);
        dfsCut(r, c);
        genBrick();
    }

    while (checkRemain());
}

void DefaultScene::genBrick() {
    int minnR = std::numeric_limits<int>::max(), minnC = std::numeric_limits<int>::max();
    int maxxR = std::numeric_limits<int>::min(), maxxC = std::numeric_limits<int>::min();
    for (int i = 0; i < Board::MAX_BOARD_SIZE; i++) {
        for (int j = 0; j < Board::MAX_BOARD_SIZE; j++) {
            if (vis[i][j] == 2) {
                minnR = std::min(minnR, i);
                minnC = std::min(minnC, j);
                maxxR = std::max(maxxR, i);
                maxxC = std::max(maxxC, j);
            }
        }
    }
    Brick brick;
    brick.n = maxxR - minnR + 1;
    brick.m = maxxC - minnC + 1;
    brick.S = 0;
    for (int i = minnR; i <= maxxR; i++) {
        for (int j = minnC; j <= maxxC; j++) {
            int r = i - minnR, c = j - minnC;
            int id = r * brick.m + c;
            if (vis[i][j] == 2) {
                vis[i][j] = 0;
                brick.S.set(id);
            }
        }
    }
    for (int i = 0; i < 8; i++) {
        int r = mt() % 8;
        if (r >> 2 & 1) {
            brick = brick.flip();
        }
        for (int j = 0; j < (r & 3); j++) {
            brick = brick.rotateClockwise();
        }
    }
    _bricks.push_back(brick);
}

bool DefaultScene::checkRemain() {
    for (int i = 0; i < Board::MAX_BOARD_SIZE; i++) {
        for (int j = 0; j < Board::MAX_BOARD_SIZE; j++) {
            if (vis[i][j] == 1) {
                dfsCut(i, j);
                genBrick();
                return true;
            }
        }
    }
    return false;
}

void DefaultScene::dfsCut(int r, int c) {
    std::priority_queue<Node> Q;
    Q.push(Node(r, c, 1000000));
    int cnt = 0;
    while (!Q.empty()) {
        auto node = Q.top();
        Q.pop();
        if (vis[node.r][node.c] == 2) continue;
        vis[node.r][node.c] = 2;
        cnt++;
        if (cnt == 20) {
            break;
        }
        for (int i = 0; i < 4; i++) {
            int nr = node.r + dr[i];
            int nc = node.c + dc[i];
            if (nr < 0 || nc < 0 || nr >= Board::MAX_BOARD_SIZE || nc >= Board::MAX_BOARD_SIZE ||
                vis[nr][nc] != 1) continue;
            Q.push(Node(nr, nc, mt() % 10000 + cnt * 1000));
        }
    }
}
