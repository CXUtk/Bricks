#include "DefaultScene.h"
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
#include "Strucures/Puzzle.h"


DLXSolver* solver;


DefaultScene::DefaultScene() {
    FILE* file = fopen("Resources/Bricks/test.in", "r");
    int n, rows, cols;
    fscanf(file, "%d%d", &rows, &cols);

    if (rows * cols > MAX_SHAPE_SIZE) {
        printf("Error: rows * columns exceed the maximum limit (%d).", MAX_SHAPE_SIZE);
        fclose(file);
        throw;
        return;
    }
    _board = std::make_shared<Board>(rows, cols);


    fscanf(file, "%d", &n);

    for (int i = 0; i < n; i++) {
        int r, c;
        char S[10005];
        fscanf(file, "%d%d", &r, &c);
        int id = 0;
        for (int i = 0; i < r; i++) {
            fscanf(file, "%s", S + id);
            id += c;
        }
        Brick brick(r, c, S);
        _bricks.push_back(brick);
    }

    for (int i = 0; i < n; i++) {
        Brick bk = _bricks[i];
        std::set<Brick> bks;
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 4; k++) {
                if (bks.count(bk)) {
                    bk = bk.rotateClockwise();
                    continue;
                }
                poses[i].push_back(bk);
                bks.insert(bk);
                bk = bk.rotateClockwise();
            }
            bk = bk.flip();
        }
    }

    fclose(file);

    _puzzleBoardSize = glm::ivec2(std::max(_board->getSize().x + 64, 400 - 16), std::max(_board->getSize().y + 64, 400 - 16));

    int width = std::max(400, _puzzleBoardSize.x + 32 + 200);
    int height = std::max(400, _puzzleBoardSize.y + 32 + 300);
    Game::GetInstance().changeWindowSize(width, height);


    Brick test(4, 4,
        ".OO."
        "OOOO"
        "O..O"
        "O..O");
    for (int i = 0; i < 100; i++)
        _bricks.push_back(test);

    for (int i = 0; i < _bricks.size(); i++) {
        _cnt[i] = 1;
    }


    generateBrickTextures();
    auto time = glfwGetTime();
    // dfs(0, _bricks);
    printf("%lf seconds\n", glfwGetTime() - time);

    // Place results
    _board->clear();

    //solve();
    // _board->place(o1, glm::ivec2(0, 0), 1, TileType::BRICK);

    _handBrick = test;
    _handBrickID = -1;
}

DefaultScene::~DefaultScene() {
    delete solver;
}

void DefaultScene::update() {
    auto& game = Game::GetInstance();
    auto input = game.getInputManager();
    auto mousePos = input->getMousePosition();

    auto pos = _board->getIndexFromMousePos(_handBrick, mousePos);
    auto shadowPos = _board->getShadowIndexFromMousePos(_handBrick, mousePos);

    //auto shadowBit = _handBrick.gBit(_board->getCols()) << (shadowPos.x * _board->getCols() + shadowPos.y);
    bool canplace = (_handBrickID != -1 && _board->canPlace(_handBrick, shadowPos) && _cnt[_handBrickID]);
    if (_handBrickID != -1)
        _board->placeShadow(_handBrick, shadowPos, canplace ? 1 : 2);


    input->beginInput();

    if (_handBrickID != -1) {
        if (input->getCurMouseDown() && !input->getOldMouseDown() && canplace && _board->mouseInside(mousePos)) {
            _board->place(_handBrick, shadowPos, _handBrickID);
            _cnt[_handBrickID]--;
        }
    }
    if (input->getCurMouseRightDown() && !input->getOldMouseRightDown() && _board->mouseInside(mousePos)) {
        int c = _board->unplace(pos);
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

    ImUI::BeginGUI();

    // 绘制砖块图的UI帧
    ImUI::BeginFrame(glm::vec2(8, 8), _puzzleBoardSize, glm::vec3(1));
    {
        auto rect = ImUI::getContainerRect();
        auto size = _board->getSize();
        _board->update(glm::ivec2((int)(rect.pos.x + rect.size.x / 2.f - size.x / 2.f), (int)(rect.pos.y + rect.size.y / 2.f - size.y / 2.f)));
        _board->draw();
        _board->clearShadow();
    }
    ImUI::EndFrame();

    // 绘制下方可用砖块的帧
    ImUI::BeginFrame(glm::vec2(8, _puzzleBoardSize.y + 8 + 16), glm::vec2(_puzzleBoardSize.x, game.getHeight() - (_puzzleBoardSize.y + 16) - 16), glm::vec3(1));
    {
        auto rect = ImUI::getContainerRect();

        static int sliderValue = 0;
        glm::ivec2 size;
        ImUI::BeginFrame(glm::vec2(16, 16), glm::vec2(rect.size.x - 32, rect.size.y - 32), glm::vec3(0));
        {
            auto rect2 = ImUI::getContainerRect();
            ImUI::BeginScrollableArea(glm::vec2(0), glm::vec2(rect2.size.x - 32, rect2.size.y), sliderValue);
            {
                size = calculateBricksList(rect2.size.x - 32 - 16);
                // Gap: 64 + 10
                int startX = 8;
                int startY = 8 - sliderValue / size.x * 74;
                for (int i = 0; i < _bricks.size(); i++) {
                    auto texture = _textures[i];
                    float scale = std::min(64.f / texture->getSize().x, 64.f / texture->getSize().y);
                    scale = std::min(scale, 1.0f);
                    scale *= 0.9f;
                    if (ImUI::img_button(texture, glm::vec2(startX, startY),
                        glm::vec2(64, 64), scale,
                        _cnt[i] > 0 ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0),
                        _handBrickID == i ? glm::vec3(1, 1, 0) : glm::vec3(0, 1, 0))) {
                        printf("%d is Clicked!\n", i);
                        _handBrick = _bricks[i];
                        _handBrickID = i;
                    }
                    startX += 74;
                    if (startX + 64 > rect2.size.x - 32) {
                        startX = 8;
                        startY += 74;
                    }
                }
            }
            ImUI::EndScrollableArea();
        }
        ImUI::EndFrame();
        if (ImUI::slider(glm::vec2(rect.size.x - 32 - 8, 8), rect.size.y - 16, _bricks.size(), sliderValue)) {
            // printf("%d\n", sliderValue);
        }
    }
    ImUI::EndFrame();



    // 绘制右侧工具栏的帧
    ImUI::BeginFrame(glm::vec2(_puzzleBoardSize.x + 8 + 16, 8), glm::vec2(200, game.getHeight() - 16), glm::vec3(1));
    {
        auto rect = ImUI::getContainerRect();

        if (ImUI::pure_button(glm::vec2(rect.size.x / 2 - 60, 8), glm::vec2(120, 50), glm::vec3(0.5, 1, 0.5), glm::vec3(0, 1, 0), "Clear", glm::vec3(0, 0, 0))) {
            _board->clear();
            for (int i = 0; i < _bricks.size(); i++) {
                _cnt[i] = 1;
            }
        }
        if (ImUI::pure_button(glm::vec2(rect.size.x / 2 - 60, 50 + 16), glm::vec2(120, 50), glm::vec3(1, 0.5, 0.5),
            glm::vec3(1, 0, 0), "Solve", glm::vec3(0, 0, 0))) {

        }
    }
    ImUI::EndFrame();
    //if (!solver->isFinished() || (!_oldFinished && solver->isFinished())) {
    //    _oldFinished = solver->isFinished();
    //    applySolverToBoard();
    //}

    //std::vector<glm::vec2> lines;
    //lines.push_back(glm::vec2(0, 0));
    //lines.push_back(glm::vec2(500, 500));
    //Game::GetInstance().getGraphics()->drawLines(lines, glm::vec3(1, 1, 1), 2);

    // game.getGraphics()->drawDirectedArrow(glm::vec2(0, 0), glm::vec2(500, 500), glm::vec3(1, 0, 0), 1);
    // Input end
    input->endInput();
    ImUI::EndGUI();
}



void DefaultScene::generateBrickTextures() {
    for (auto& b : _bricks) {
        _textures.push_back(b.generateTexture(glm::vec3(1, 1, 1)));
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

//static int dr[4] = { 1, -1, 0, 0 };
//static int dc[4] = { 0, 0, 1, -1 };
//// 0 代表空格子，1代表当前没被切割的格子，2代表当前被切割的格子
//static int vis[100][100];
//static std::mt19937 mt;
//
//
//bool found = false;

//void DefaultScene::dfs(int x, std::vector<Brick>& bricks) {
//    if (x == bricks.size()) {
//        found = true;
//        return;
//    }
//    int r = _board->getRows(), c = _board->getCols();
//    // Brick b = bricks[x];
//
//
//    // 剪枝1：如果剩余空间的未放置方块最大大小小于这个块的大小，剪掉
//    //if (!_board->testCanPlace(b)) return;
//    // 储存格式：0-7位存砖块ID，8-11位存，12-32存坐标
//
//    int i = 0;
//    for (auto& b : poses[x]) {
//        auto gbit = b.gBit(_board->getCols());
//        for (int j = 0; j <= r - b.n; j++) {
//            for (int k = 0; k <= c - b.m; k++) {
//                auto coord = glm::ivec2(j, k);
//                auto judgeBit = gbit << (j * c + k);
//                if (_board->canPlace(judgeBit, coord)) {
//                    _board->placeBit(judgeBit);
//
//                    int id = j * c + k;
//                    int info = x;
//                    info <<= 3;
//                    info |= i;
//                    info <<= 20;
//                    info |= id;
//                    placeInfo.push_back(info);
//
//                    //  _board->place(b, coord, x, TileType::BRICK);
//                    dfs(x + 1, bricks);
//                    if (found)return;
//
//                    placeInfo.pop_back();
//                    _board->unplaceBit(judgeBit);
//                    //_board->remove(b, coord);
//                }
//            }
//        }
//        i++;
//    }
//}



void DefaultScene::solve() {

    //int n = _bricks.size();
    //printf("Number of bricks: %d\n", n);

    //int curSum = 0;
    //for (int i = 0; i < _bricks.size(); i++) {
    //    curSum += _bricks[i].S.count();
    //}

    //int totSize = Board::MAX_BOARD_SIZE * Board::MAX_BOARD_SIZE;
    ////for (int i = 0; i < totSize - curSum; i++) {
    ////    _bricks.insert(_bricks.begin(), Brick(1, 1, "O"));
    ////}

    //// 列号 1 ~ n 是砖块的ID，n + 1 ~ n + 100 是格子ID约束
    //solver = new DLXSolver(n * 8 * totSize, n + totSize, totSize - curSum);
    //// 储存格式：0-7位存砖块ID，8-11位存，12-32存坐标

    //int tot = 0;

    //for (int i = 0; i < _bricks.size(); i++) {
    //    int col = 1 + i;
    //    Brick b = _bricks[i];
    //    b.gBit();

    //    std::set<Brick> bricks;
    //    for (int j = 0; j < 2; j++) {
    //        for (int k = 0; k < 4; k++) {
    //            if (bricks.count(b))continue;
    //            bricks.insert(b);
    //            for (int r = 0; r <= Board::MAX_BOARD_SIZE - b.n; r++) {
    //                for (int c = 0; c <= Board::MAX_BOARD_SIZE - b.m; c++) {
    //                    int id = r * Board::MAX_BOARD_SIZE + c;
    //                    int info = i;
    //                    info <<= 3;
    //                    info |= ((j << 2) | k);
    //                    info <<= 20;
    //                    info |= id;
    //                    _idMap[++tot] = info;

    //                    auto res = b.G << id;
    //                    solver->link(tot, 1 + i);
    //                    for (int l = 0; l < Board::MAX_BOARD_SIZE * Board::MAX_BOARD_SIZE; l++) {
    //                        if (res.test(l)) {
    //                            solver->link(tot, l + n + 1);
    //                        }
    //                    }
    //                }
    //            }
    //            b = b.rotateClockwise();
    //        }
    //        b = b.flip();
    //    }
    //}
    ////solver->solve();
    //_solverThread = new std::thread([]() {
    //    auto time = glfwGetTime();
    //    solver->solve();
    //    printf("%lf seconds\n", glfwGetTime() - time);
    //    });
    //// 传入idMap为了追踪砖块信息
}

// 计算可选物块列表的行数和列数
glm::ivec2 DefaultScene::calculateBricksList(int maxWidth) {
    glm::ivec2 output(0);
    int x = 0;
    int y = 0;
    for (int i = 0; i < _bricks.size(); i++) {
        x += 64;
        if (x > maxWidth) {
            if (output.x == 0)
                output.x = i;
            y++;
            x = 0;
        }
        else {
            x += 10;
        }
    }
    output.y = y;
    if (output.x == 0) output.x = 1;
    return output;
}





//
//void DefaultScene::randomGenerate() {
//    mt.seed(19284321);
//    for (int i = 0; i < _board->getRows(); i++) {
//        for (int j = 0; j < _board->getCols(); j++) {
//            vis[i][j] = 1;
//        }
//    }
//    cutBoard();
//    generateBrickTextures();
//}
//
//void DefaultScene::cutBoard() {
//    for (int i = 0; i < 4; i++) {
//        int r, c;
//        do {
//            r = mt() % _board->getRows();
//            c = mt() % _board->getCols();
//
//        } while (vis[r][c] != 1);
//        dfsCut(r, c);
//        genBrick();
//    }
//
//    while (checkRemain());
//}
//
//void DefaultScene::genBrick() {
//    int minnR = std::numeric_limits<int>::max(), minnC = std::numeric_limits<int>::max();
//    int maxxR = std::numeric_limits<int>::min(), maxxC = std::numeric_limits<int>::min();
//    for (int i = 0; i < _board->getRows(); i++) {
//        for (int j = 0; j < _board->getCols(); j++) {
//            if (vis[i][j] == 2) {
//                minnR = std::min(minnR, i);
//                minnC = std::min(minnC, j);
//                maxxR = std::max(maxxR, i);
//                maxxC = std::max(maxxC, j);
//            }
//        }
//    }
//    Brick brick;
//    brick.n = maxxR - minnR + 1;
//    brick.m = maxxC - minnC + 1;
//    brick.S = 0;
//    for (int i = minnR; i <= maxxR; i++) {
//        for (int j = minnC; j <= maxxC; j++) {
//            int r = i - minnR, c = j - minnC;
//            int id = r * brick.m + c;
//            if (vis[i][j] == 2) {
//                vis[i][j] = 0;
//                brick.S.set(id);
//            }
//        }
//    }
//    for (int i = 0; i < 8; i++) {
//        int r = mt() % 8;
//        if (r >> 2 & 1) {
//            brick = brick.flip();
//        }
//        for (int j = 0; j < (r & 3); j++) {
//            brick = brick.rotateClockwise();
//        }
//    }
//    _bricks.push_back(brick);
//}
//
//bool DefaultScene::checkRemain() {
//    for (int i = 0; i < _board->getRows(); i++) {
//        for (int j = 0; j < _board->getCols(); j++) {
//            if (vis[i][j] == 1) {
//                dfsCut(i, j);
//                genBrick();
//                return true;
//            }
//        }
//    }
//    return false;
//}
//
//void DefaultScene::dfsCut(int r, int c) {
//    std::priority_queue<Node> Q;
//    Q.push(Node(r, c, 1000000));
//    int cnt = 0;
//    while (!Q.empty()) {
//        auto node = Q.top();
//        Q.pop();
//        if (vis[node.r][node.c] == 2) continue;
//        vis[node.r][node.c] = 2;
//        cnt++;
//        if (cnt == 20) {
//            break;
//        }
//        for (int i = 0; i < 4; i++) {
//            int nr = node.r + dr[i];
//            int nc = node.c + dc[i];
//            if (nr < 0 || nc < 0 || nr >= _board->getRows() || nc >= _board->getCols() ||
//                vis[nr][nc] != 1) continue;
//            Q.push(Node(nr, nc, mt() % 10000 + cnt * 1000));
//        }
//    }
//}
