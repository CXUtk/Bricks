﻿#include "DefaultScene.h"
#include "Game.h"
#include "ImUI/ImUI.h"
#include "Utils.h"
#include "Algorithm/DLXSolver.h"
#include <glm/gtx/transform.hpp>

#include <random>
#include <queue>
#include <string>
#include <vector>
#include <algorithm>
#include <set>


DLXSolver* solver;

DefaultScene::DefaultScene() {
    _board = std::make_shared<Board>(Board::MAX_BOARD_SIZE, Board::MAX_BOARD_SIZE, glm::vec2(250, 250));
    Brick fill(1, 1, "O");
    Brick o1(3, 3,
        ".O."
        "OOO"
        "O.O");
    Brick orz(2, 5,
        ".OOO."
        "OO.OO");
    Brick full(5, 5,
        "OOOOO"
        "OOOOO"
        "OOOOO"
        "OOO.O"
        "OO...");
    Brick o2(3, 3,
        ".O."
        "OOO"
        ".O.");
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
    //randomGenerate();
    for (int i = 0; i < 14; i++) {
        _bricks.push_back(orz);
    }
    _bricks.push_back(o1);

    //_bricks.push_back(o2);
    generateBrickTextures();

    //_bricks.push_back(o1);
    //_bricks.push_back(o2);
    //_bricks.push_back(o3);
    //_bricks.push_back(o4);
    //_bricks.push_back(o4);
    //_bricks.push_back(o4);
    //_bricks.push_back(o5);
    //_bricks.push_back(o5);
    //_bricks.push_back(o5);
    ////generateBrickTextures();
    //Brick b;
    //b.m = 11;
    //b.n = 11;
    //b.S = 0;
    //for (int i = 0; i < 121; i++)
    //    b.S.set(i);
    //_bricks.push_back(b);
    //// _bricks.push_back(b);
    //generateBrickTextures();
    /*  randomGenerate();*/



    for (int i = 0; i < _bricks.size(); i++) {
        _cnt[i] = 1;
    }

    auto time = glfwGetTime();
    dfs(0, _bricks);
    // solve();
    // _board->place(o1, glm::ivec2(0, 0), 1, TileType::BRICK);
    printf("%lf seconds\n", glfwGetTime() - time);
    Brick test(4, 4,
        ".OO."
        "OOOO"
        "O..O"
        "O..O");

    _handBrick = test;
    _handBrickID = -1;
}

DefaultScene::~DefaultScene() {
    delete solver;
    delete _solverThread;
}

void DefaultScene::update() {
    _board->update();
    auto& game = Game::GetInstance();
    auto input = game.getInputManager();
    auto mousePos = input->getMousePosition();
    mousePos.y = game.getHeight() - mousePos.y;

    auto pos = _board->getIndexFromMousePos(_handBrick, mousePos);
    auto shadowPos = _board->getShadowIndexFromMousePos(_handBrick, mousePos);
    bool canplace = (_handBrickID != -1 && _board->canPlace(_handBrick, shadowPos) && _cnt[_handBrickID]);
    if (_handBrickID != -1)
        _board->placeShadow(_handBrick, shadowPos, canplace ? 1 : 2);


    input->beginInput();
    if (input->getCurMouseDown() && !input->getOldMouseDown() && canplace && _board->mouseInside(mousePos)) {
        _board->place(_handBrick, shadowPos, _handBrickID, TileType::BRICK);
        _cnt[_handBrickID]--;
    }
    if (input->getCurMouseRightDown() && !input->getOldMouseRightDown() && _board->mouseInside(mousePos)) {
        int c = _board->unplace(pos);
        printf("%d %d\n", pos.x, pos.y);
        if (c != -1) {
            _cnt[c]++;
        }
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

    auto res = solver->getIntermidiateResult();
    for (auto a : res) {
        int info = _idMap[a];
        int id = info & ((1 << 20) - 1);
        int r = id / Board::MAX_BOARD_SIZE;
        int c = id % Board::MAX_BOARD_SIZE;
        info >>= 20;
        int state = info & 7;
        info >>= 3;
        int x = info & 0xff;
        Brick b = _bricks[x];
        if (state >> 2 & 1) {
            b = b.flip();
        }
        for (int i = 0; i < (state & 3); i++) {
            b = b.rotateClockwise();
        }
        _board->place(b, glm::ivec2(r, c), x, TileType::BRICK);
    }

    _board->draw();
    _board->clearShadow();

    for (auto a : res) {
        int info = _idMap[a];
        int id = info & ((1 << 20) - 1);
        int r = id / Board::MAX_BOARD_SIZE;
        int c = id % Board::MAX_BOARD_SIZE;
        info >>= 20;
        int state = info & 7;
        info >>= 3;
        int x = info & 0xff;
        Brick b = _bricks[x];
        if (state >> 2 & 1) {
            b = b.flip();
        }
        for (int i = 0; i < (state & 3); i++) {
            b = b.rotateClockwise();
        }
        _board->remove(b, glm::ivec2(r, c));
    }

    // Gap: 64 + 10
    int startX = 32;
    int startY = 200;
    for (int i = 0; i < _bricks.size(); i++) {
        auto texture = _textures[i];
        float scale = std::min(64.f / texture->getSize().x, 64.f / texture->getSize().y);
        scale = std::min(scale, 1.0f);
        scale *= 0.9f;
        if (ImUI::GetInstance().img_button(texture, glm::vec2(startX, startY),
            glm::vec2(64, 64), scale,
            _cnt[i] > 0 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0),
            _handBrickID == i ? glm::vec3(1, 1, 0) : glm::vec3(0, 1, 0))) {
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


bool found = false;
void DefaultScene::dfs(int x, std::vector<Brick>& bricks) {
    if (x == bricks.size()) {
        found = true;
        return;
    }
    int r = _board->getRows(), c = _board->getCols();
    Brick b = bricks[x];
    b.gBit();

    // 剪枝1：如果剩余空间的未放置方块最大大小小于这个块的大小，剪掉
    //if (!_board->testCanPlace(b)) return;

    std::set<Brick> bks;
    for (int s = 0; s < 2; s++) {
        for (int i = 0; i < 4; i++) {
            if (bks.count(b))continue;
            bks.insert(b);
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



void DefaultScene::solve() {

    int n = _bricks.size();
    printf("Number of bricks: %d\n", n);

    int curSum = 0;
    for (int i = 0; i < _bricks.size(); i++) {
        curSum += _bricks[i].S.count();
    }

    int totSize = Board::MAX_BOARD_SIZE * Board::MAX_BOARD_SIZE;

    // 列号 1 ~ n 是砖块的ID，n + 1 ~ n + 100 是格子ID约束
    solver = new DLXSolver(n * 8 * totSize, n + totSize, totSize - curSum);
    // 储存格式：0-7位存砖块ID，8-11位存，12-32存坐标

    int tot = 0;

    for (int i = 0; i < _bricks.size(); i++) {
        int col = 1 + i;
        Brick b = _bricks[i];
        b.gBit();

        std::set<Brick> bricks;
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 4; k++) {
                if (bricks.count(b))continue;
                bricks.insert(b);
                for (int r = 0; r <= Board::MAX_BOARD_SIZE - b.n; r++) {
                    for (int c = 0; c <= Board::MAX_BOARD_SIZE - b.m; c++) {
                        int id = r * Board::MAX_BOARD_SIZE + c;
                        int info = i;
                        info <<= 3;
                        info |= ((j << 2) | k);
                        info <<= 20;
                        info |= id;
                        _idMap[++tot] = info;

                        auto res = b.G << id;
                        solver->link(tot, 1 + i);
                        for (int l = 0; l < Board::MAX_BOARD_SIZE * Board::MAX_BOARD_SIZE; l++) {
                            if (res.test(l)) {
                                solver->link(tot, l + n + 1);
                            }
                        }
                    }
                }
                b = b.rotateClockwise();
            }
            b = b.flip();
        }
    }
    //solver->solve();
    _solverThread = new std::thread([]() {
        solver->solve();
        });
    // 传入idMap为了追踪砖块信息
}



void DefaultScene::randomGenerate() {
    mt.seed(19284321);
    for (int i = 0; i < Board::MAX_BOARD_SIZE; i++) {
        for (int j = 0; j < Board::MAX_BOARD_SIZE; j++) {
            vis[i][j] = 1;
        }
    }
    cutBoard();
    generateBrickTextures();
}

void DefaultScene::generateBrickTextures() {
    for (auto& b : _bricks) {
        _textures.push_back(b.generateTexture(glm::vec3(1, 1, 1)));
    }
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
    brick.gBit();
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
