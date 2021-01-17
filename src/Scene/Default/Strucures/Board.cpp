#include "Board.h"
#include "Game.h"
#include <algorithm>
#include <queue>
#include <glm/gtx/transform.hpp>

static glm::vec3 colors[] = {
    glm::vec3(1, 0, 1),
    glm::vec3(1, 0, 0),
    glm::vec3(0.3, 0.7, 0.5),
    glm::vec3(0, 0.9, 0),
    glm::vec3(0, 0.8, 0),
    glm::vec3(1, 0.8, 0),
    glm::vec3(0, 1, 0),
    glm::vec3(0, 0.9, 0.9),
    glm::vec3(0.3, 0.8, 0.1),
    glm::vec3(0.9, 0.5, 0.5),
};

Board::Board(int rows, int columns) :_rows(rows), _columns(columns) {
    tiles = new Tile[_rows * _columns];
    for (int i = 0; i < _rows * _columns; i++) {
        tiles[i].id = -1;
        tiles[i].color = 0;
        tiles[i].shadow = 0;
    }
}
void Board::place(const Shape& brick, glm::ivec2 pos, int id, int color) {
    for (int i = pos.x; i < std::min(pos.x + brick.rows, _rows); i++) {
        for (int j = pos.y; j < std::min(pos.y + brick.cols, _columns); j++) {
            int r = i - pos.x, c = j - pos.y;
            if (brick.get(r, c)) {
                setTile(i, j, id, color);
            }
        }
    }
}

//void Board::placeBit(std::bitset<128> bitset) {
//    assert((bitset & S) == 0);
//    S |= bitset;
//}
//
//void Board::unplaceBit(std::bitset<128> bitset) {
//    assert((S & bitset) == bitset);
//    S ^= bitset;
//}

static int dr[4] = { 1, -1, 0, 0 };
static int dc[4] = { 0, 0, 1, -1 };
std::bitset<MAX_SHAPE_SIZE> Board::unplace(glm::ivec2 pos, int& id) {
    std::queue<glm::ivec2> Q;
    Q.push(pos);
    std::bitset<MAX_SHAPE_SIZE> S(0);
    id = getTileID(pos.x, pos.y);
    if (id == -1) return S;
    while (!Q.empty()) {
        auto p = Q.front();
        setTile(p.x, p.y, -1, 0);
        S.set(p.x * _columns + p.y);
        Q.pop();
        for (int i = 0; i < 4; i++) {
            int nr = p.x + dr[i];
            int nc = p.y + dc[i];
            if (nr < 0 || nc < 0 || nr >= _rows || nc >= _columns
                || getTileID(nr, nc) == -1 || getTileID(nr, nc) != id)
                continue;
            Q.push(glm::ivec2(nr, nc));
        }
    }
    return S;
}

void Board::placeShadow(const Shape& brick, glm::ivec2 pos, int color) {
    for (int i = pos.x; i < std::min(pos.x + brick.rows, _rows); i++) {
        for (int j = pos.y; j < std::min(pos.y + brick.cols, _columns); j++) {
            int r = i - pos.x, c = j - pos.y;
            if (brick.get(r, c)) {
                tiles[i * _columns + j].shadow = color;
            }
        }
    }
}


bool Board::canPlace(const Shape& brick, glm::ivec2 pos) {
    for (int i = pos.x; i < std::min(pos.x + brick.rows, _rows); i++) {
        for (int j = pos.y; j < std::min(pos.y + brick.cols, _columns); j++) {
            int r = i - pos.x, c = j - pos.y;
            if (brick.get(r, c) && getTileID(i, j) != -1) {
                return false;
            }
        }
    }
    return true;
}

glm::ivec2 Board::getIndexFromMousePos(const Shape& brick, glm::vec2 pos) const {

    pos -= _topLeft;// glm::vec2(_topLeft.x + halfSize.x, _topLeft.y + halfSize.y);

    int r = std::max(0, std::min(_rows - 1, (int)(pos.y / BLOCK_SIZE)));
    int c = std::max(0, std::min(_columns - 1, (int)(pos.x / BLOCK_SIZE)));
    return glm::ivec2(r, c);
}

glm::ivec2 Board::getShadowIndexFromMousePos(const Shape& brick, glm::vec2 pos) const {
    auto halfSize = glm::ivec2(brick.cols, brick.rows) * 16;
    pos -= glm::vec2(_topLeft.x + halfSize.x, _topLeft.y + halfSize.y);

    int r = std::max(0, std::min(_rows - 1, (int)(pos.y / BLOCK_SIZE)));
    r = std::min(r, _rows - brick.rows);
    int c = std::max(0, std::min(_columns - 1, (int)(pos.x / BLOCK_SIZE)));
    c = std::min(c, _columns - brick.cols);
    return glm::ivec2(r, c);
}

bool Board::mouseInside(glm::vec2 mousePos) const {
    return mousePos.x >= _topLeft.x && mousePos.x <= _topLeft.x + _columns * BLOCK_SIZE
        && mousePos.y >= _topLeft.y && mousePos.y <= _topLeft.y + _rows * BLOCK_SIZE;
}



//static int dr[4] = { 1, -1, 0, 0 };
//static int dc[4] = { 0, 0, 1, -1 };
// 0 代表空格子，1代表当前没被切割的格子，2代表当前被切割的格子
static int vis[100][100];



void Board::clearShadow() {
    for (int i = 0; i < _rows * _columns; i++) {
        tiles[i].shadow = 0;
    }
}

void Board::update(glm::ivec2 topLeft) {
    _topLeft = topLeft;
}

void Board::draw() {
    auto& game = Game::GetInstance();

    auto pos = glm::vec2(_topLeft.x, _topLeft.y);
    game.getGraphics()->drawQuad(pos, glm::vec2(BLOCK_SIZE * _columns, BLOCK_SIZE * _rows), glm::vec3(1, 1, 1));

    std::vector<Segment> lines;
    for (int i = 0; i <= _rows; i++) {
        glm::vec2 start = glm::vec2(_topLeft.x, _topLeft.y + i * BLOCK_SIZE);
        glm::vec2 end = glm::vec2(_topLeft.x + _columns * BLOCK_SIZE, _topLeft.y + i * BLOCK_SIZE);

        lines.push_back(Segment(start, end));
    }

    for (int i = 0; i <= _columns; i++) {
        glm::vec2 start = glm::vec2(_topLeft.x + i * BLOCK_SIZE, _topLeft.y);
        glm::vec2 end = glm::vec2(_topLeft.x + i * BLOCK_SIZE, _topLeft.y + _rows * BLOCK_SIZE);

        lines.push_back(Segment(start, end));
    }

    game.getGraphics()->drawLines(lines, glm::vec3(0, 0, 0), 1);

    std::vector<Triangle> triangles;
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _columns; j++) {
            int color = getTileID(i, j);
            if (color != -1) {
                //glm::vec2 bl = glm::vec2(_topLeft.x + j * BLOCK_SIZE, _topLeft.y + i * BLOCK_SIZE);
                //game.getGraphics()->drawQuad(bl, glm::vec2(BLOCK_SIZE), colors[color % 10]);
                drawCell(i, j, game.getGraphics());
            }
        }
    }

    //game.getGraphics()->drawLines(edges, glm::vec3(0, 0, 0), 1);
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _columns; j++) {
            int s = getShadow(i, j);
            if (s) {
                glm::vec2 bl = glm::vec2(_topLeft.x + j * BLOCK_SIZE, _topLeft.y + i * BLOCK_SIZE);
                auto color = (s == 1) ? glm::vec3(0.5, 1, 0.5) : glm::vec3(1, 0.5, 0.5);
                game.getGraphics()->drawQuad(bl, glm::vec2(BLOCK_SIZE), color);
            }
        }
    }

}

void Board::remove(const Shape& brick, glm::ivec2 pos) {
    for (int i = pos.x; i < std::min(pos.x + brick.rows, _rows); i++) {
        for (int j = pos.y; j < std::min(pos.y + brick.cols, _columns); j++) {
            int r = i - pos.x, c = j - pos.y;
            if (brick.get(r, c)) {
                setTile(i, j, -1, 0);
            }
        }
    }
}

void Board::clear(const std::bitset<MAX_SHAPE_SIZE>& mask) {
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _columns; j++) {
            if (!mask[i * _columns + j])
                setTile(i, j, -1, 0);
        }
    }
}

glm::ivec2 Board::getSize() const {
    return glm::ivec2(_columns, _rows) * BLOCK_SIZE;
}

void Board::drawCell(int r, int c, std::shared_ptr<Graphics> graphic) {
    glm::vec2 bl = glm::vec2(_topLeft.x + c * BLOCK_SIZE, _topLeft.y + r * BLOCK_SIZE);
    auto& game = Game::GetInstance();
    auto height = game.getHeight();

    // 下，上，右，左
    static int dr[4] = { 1, -1, 0, 0 };
    static int dc[4] = { 0, 0, 1, -1 };

    int color = getTileID(r, c);

    std::vector<Triangle> drakTri;
    std::vector<Triangle> lightTri;

    auto judge = [=](int i, int j) {
        return i >= 0 && j >= 0 && i < _rows&& j < _columns&& getTileID(i, j) == color;
    };

    // 上方，左边是否有联通块
    int t = judge(r - 1, c);
    t |= judge(r, c - 1) << 1;
    t |= judge(r - 1, c - 1) << 2;

    int t2 = judge(r + 1, c);
    t2 |= judge(r, c + 1) << 1;
    glm::vec3 drawColor;
    if (color == -2) {
        drawColor = glm::vec3(0.2, 0.2, 0.2);
    }
    else {
        drawColor = colors[color % 10];
    }
    for (int i = 0; i < 4; i++) {
        int nr = r + dr[i];
        int nc = c + dc[i];
        if (!judge(nr, nc)) {
            if (i == 0) {
                drakTri.push_back(Triangle(
                    glm::vec4(bl.x, bl.y + BLOCK_SIZE, 0, 0),
                    glm::vec4(bl.x + BLOCK_SIZE - 4, bl.y + BLOCK_SIZE - 4, 0, 0),
                    glm::vec4((t & 2) ? bl.x - 4 : bl.x + 4, bl.y + BLOCK_SIZE - 4, 0, 0)
                ));
                drakTri.push_back(Triangle(
                    glm::vec4(bl.x, bl.y + BLOCK_SIZE, 0, 0),
                    glm::vec4(bl.x + BLOCK_SIZE, bl.y + BLOCK_SIZE, 0, 0),
                    glm::vec4(bl.x + BLOCK_SIZE - 4, bl.y + BLOCK_SIZE - 4, 0, 0)
                ));
                if (t2 & 2) {
                    drakTri.push_back(Triangle(
                        glm::vec4(bl.x + BLOCK_SIZE - 4, bl.y + BLOCK_SIZE - 4, 0, 0),
                        glm::vec4(bl.x + BLOCK_SIZE, bl.y + BLOCK_SIZE, 0, 0),
                        glm::vec4(bl.x + BLOCK_SIZE + 4, bl.y + BLOCK_SIZE - 4, 0, 0)
                    ));
                }
            }
            else if (i == 1) {
                lightTri.push_back(Triangle(
                    glm::vec4(bl.x, bl.y, 0, 0),
                    glm::vec4((t & 2) ? bl.x - 4 : bl.x + 4, bl.y + 4, 0, 0),
                    glm::vec4(bl.x + BLOCK_SIZE - 4, bl.y + 4, 0, 0)
                ));
                lightTri.push_back(Triangle(
                    glm::vec4(bl.x, bl.y, 0, 0),
                    glm::vec4(bl.x + BLOCK_SIZE - 4, bl.y + 4, 0, 0),
                    glm::vec4(bl.x + BLOCK_SIZE, bl.y, 0, 0)
                ));
                if (t2 & 2) {
                    lightTri.push_back(Triangle(
                        glm::vec4(bl.x + BLOCK_SIZE - 4, bl.y + 4, 0, 0),
                        glm::vec4(bl.x + BLOCK_SIZE + 4, bl.y + 4, 0, 0),
                        glm::vec4(bl.x + BLOCK_SIZE, bl.y, 0, 0)
                    ));
                }
            }
            else if (i == 2) {
                drakTri.push_back(Triangle(
                    glm::vec4(bl.x + BLOCK_SIZE, bl.y, 0, 0),
                    glm::vec4(bl.x + BLOCK_SIZE - 4, (t & 1) ? bl.y - 4 : bl.y + 4, 0, 0),
                    glm::vec4(bl.x + BLOCK_SIZE - 4, bl.y + BLOCK_SIZE - 4, 0, 0)
                ));
                drakTri.push_back(Triangle(
                    glm::vec4(bl.x + BLOCK_SIZE, bl.y, 0, 0),
                    glm::vec4(bl.x + BLOCK_SIZE - 4, bl.y + BLOCK_SIZE - 4, 0, 0),
                    glm::vec4(bl.x + BLOCK_SIZE, bl.y + BLOCK_SIZE, 0, 0)
                ));
                if (t2 & 1) {
                    drakTri.push_back(Triangle(
                        glm::vec4(bl.x + BLOCK_SIZE - 4, bl.y + BLOCK_SIZE - 4, 0, 0),
                        glm::vec4(bl.x + BLOCK_SIZE - 4, bl.y + BLOCK_SIZE + 4, 0, 0),
                        glm::vec4(bl.x + BLOCK_SIZE, bl.y + BLOCK_SIZE, 0, 0)
                    ));
                }
            }
            else if (i == 3) {
                lightTri.push_back(Triangle(
                    glm::vec4(bl.x, bl.y, 0, 0),
                    glm::vec4(bl.x + 4, bl.y + BLOCK_SIZE - 4, 0, 0),
                    glm::vec4(bl.x + 4, (t & 1) ? bl.y - 4 : bl.y + 4, 0, 0)
                ));
                lightTri.push_back(Triangle(
                    glm::vec4(bl.x, bl.y, 0, 0),
                    glm::vec4(bl.x, bl.y + BLOCK_SIZE, 0, 0),
                    glm::vec4(bl.x + 4, bl.y + BLOCK_SIZE - 4, 0, 0)
                ));
                if (t2 & 1) {
                    lightTri.push_back(Triangle(
                        glm::vec4(bl.x + 4, bl.y + BLOCK_SIZE - 4, 0, 0),
                        glm::vec4(bl.x, bl.y + BLOCK_SIZE, 0, 0),
                        glm::vec4(bl.x + 4, bl.y + BLOCK_SIZE + 4, 0, 0)
                    ));
                }

            }

        }

        game.getGraphics()->drawTriangles(lightTri, glm::identity<glm::mat4>(), drawColor + glm::vec3(0.5));
        game.getGraphics()->drawTriangles(drakTri, glm::identity<glm::mat4>(), drawColor * 0.4f);
    }
    // 三个方向全都有块
    if (t == 7) {
        game.getGraphics()->drawQuad(glm::vec2(bl.x - BLOCK_SIZE + 4, bl.y - BLOCK_SIZE + 4), glm::vec2(BLOCK_SIZE * 2 - 8), drawColor);
    }
    else {
        // 有上方物块
        if (t & 1) {
            game.getGraphics()->drawQuad(glm::vec2(bl.x + 4, bl.y - 4), glm::vec2(BLOCK_SIZE - 8, BLOCK_SIZE), drawColor);
        }
        // 有左边物块
        if (t & 2) {
            game.getGraphics()->drawQuad(glm::vec2(bl.x - 4, bl.y + 4), glm::vec2(BLOCK_SIZE, BLOCK_SIZE - 8), drawColor);
        }
        game.getGraphics()->drawQuad(glm::vec2(bl.x + 4, bl.y + 4), glm::vec2(BLOCK_SIZE - 8), drawColor);
    }
}

void Board::setTile(int r, int c, int k, int color) {
    int id = r * _columns + c;
    tiles[id].id = k;
    tiles[id].color = color;
}

int Board::getTileID(int r, int c) const {
    int id = r * _columns + c;
    return tiles[id].id;
}

int Board::getShadow(int r, int c) const {
    int id = r * _columns + c;
    return tiles[id].shadow;
}
