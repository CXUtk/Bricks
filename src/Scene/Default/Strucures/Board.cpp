#include "Board.h"
#include "Game.h"
#include <algorithm>



Brick::Brick(int n, int m) : n(n), m(m) {
}

Brick::Brick(int n, int m, const char* shape) : n(n), m(m) {
    S = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int id = i * m + j;
            if (shape[id] == 'O') {
                S.set(id);
            }
        }
    }
}

Brick::Brick(int n, int m, std::bitset<128> set) : n(n), m(m) {
    S = set;
}

Brick Brick::flip() const {
    Brick out(n, m);
    out.S = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int id = i * m + j;
            if (S.test(id)) {
                int id2 = i * m + (m - j - 1);
                out.S.set(id2);
            }
        }
    }
    return out;
}

Brick Brick::rotateClockwise() const {
    Brick out(m, n);
    out.S = 0;
    for (int i = n - 1; i >= 0; i--) {
        for (int j = 0; j < m; j++) {
            int id = i * m + j;
            if (S.test(id)) {
                int id2 = j * n + n - i - 1;
                out.S.set(id2);
            }
        }
    }
    return out;
}


Board::Board(int rows, int columns, glm::vec2 center) :_rows(rows), _columns(columns), _center(center) {
    memset(tiles, 0, sizeof(tiles));
    memset(shadow, 0, sizeof(shadow));
}

void Board::place(const Brick& brick, glm::ivec2 pos, int id, TileType type) {
    for (int i = pos.x; i < std::min(pos.x + brick.n, MAX_BOARD_SIZE); i++) {
        for (int j = pos.y; j < std::min(pos.y + brick.m, MAX_BOARD_SIZE); j++) {
            int r = i - pos.x, c = j - pos.y;
            if (brick.get(r, c)) {
                tiles[i][j].type = type;
                tiles[i][j].color = id;
            }
        }
    }
}

void Board::placeShadow(const Brick& brick, glm::ivec2 pos, int color) {
    for (int i = pos.x; i < std::min(pos.x + brick.n, MAX_BOARD_SIZE); i++) {
        for (int j = pos.y; j < std::min(pos.y + brick.m, MAX_BOARD_SIZE); j++) {
            int r = i - pos.x, c = j - pos.y;
            if (brick.get(r, c)) {
                shadow[i][j] = color;
            }
        }
    }
}


bool Board::canPlace(const Brick& brick, glm::ivec2 pos) {
    if (pos.x + brick.n > MAX_BOARD_SIZE || pos.y + brick.m > MAX_BOARD_SIZE) return false;
    for (int i = pos.x; i < std::min(pos.x + brick.n, MAX_BOARD_SIZE); i++) {
        for (int j = pos.y; j < std::min(pos.y + brick.m, MAX_BOARD_SIZE); j++) {
            int r = i - pos.x, c = j - pos.y;
            if (brick.get(r, c) && tiles[i][j].type != TileType::EMPTY) {
                return false;
            }
        }
    }
    return true;
}

glm::ivec2 Board::getIndexFromPos(glm::vec2 pos) const {
    pos -= _topLeft;
    return glm::ivec2(std::max(0, std::min(MAX_BOARD_SIZE - 1, (int)(pos.y / BLOCK_SIZE))), std::max(0, std::min(MAX_BOARD_SIZE - 1, (int)(pos.x / BLOCK_SIZE))));
}

void Board::clearShadow() {
    memset(shadow, 0, sizeof(shadow));
}

void Board::update() {
    auto& game = Game::GetInstance();
    _topLeft = _center - glm::vec2(_rows * 0.5f * BLOCK_SIZE, _columns * 0.5f * BLOCK_SIZE);
}

void Board::draw() {
    auto& game = Game::GetInstance();
    auto pos = glm::vec2(_topLeft.x, _topLeft.y + BLOCK_SIZE * MAX_BOARD_SIZE);
    auto height = game.getHeight();
    pos.y = height - pos.y;
    game.getGraphics()->drawQuad(pos, glm::vec2(BLOCK_SIZE * MAX_BOARD_SIZE), glm::vec3(1, 1, 1));

    std::vector<glm::vec2> lines;
    for (int i = 0; i <= _rows; i++) {
        glm::vec2 start = glm::vec2(_topLeft.x, _topLeft.y + i * BLOCK_SIZE);
        start.y = height - start.y;
        glm::vec2 end = glm::vec2(_topLeft.x + _columns * BLOCK_SIZE, _topLeft.y + i * BLOCK_SIZE);
        end.y = height - end.y;

        lines.push_back(start);
        lines.push_back(end);
    }

    for (int i = 0; i <= _columns; i++) {
        glm::vec2 start = glm::vec2(_topLeft.x + i * BLOCK_SIZE, _topLeft.y);
        start.y = height - start.y;
        glm::vec2 end = glm::vec2(_topLeft.x + i * BLOCK_SIZE, _topLeft.y + _rows * BLOCK_SIZE);
        end.y = height - end.y;

        lines.push_back(start);
        lines.push_back(end);
    }

    game.getGraphics()->drawLines(lines, glm::vec3(0, 0, 0), 1);

    std::vector<glm::vec2> edges;
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _columns; j++) {
            drawCell(i, j, game.getGraphics(), edges);
        }
    }
    //game.getGraphics()->drawLines(edges, glm::vec3(0, 0, 0), 1);

    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _columns; j++) {
            if (shadow[i][j]) {
                glm::vec2 bl = glm::vec2(_topLeft.x + j * BLOCK_SIZE, _topLeft.y + (i + 1) * BLOCK_SIZE);
                bl.y = height - bl.y;
                auto color = (shadow[i][j] == 1) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
                game.getGraphics()->drawQuad(bl, glm::vec2(BLOCK_SIZE), color);
            }
        }
    }

}

void Board::remove(const Brick& brick, glm::ivec2 pos) {
    for (int i = pos.x; i < std::min(pos.x + brick.n, MAX_BOARD_SIZE); i++) {
        for (int j = pos.y; j < std::min(pos.y + brick.m, MAX_BOARD_SIZE); j++) {
            int r = i - pos.x, c = j - pos.y;
            if (brick.get(r, c)) {
                tiles[i][j].type = TileType::EMPTY;
            }
        }
    }
}

void Board::drawCell(int r, int c, std::shared_ptr<Graphics> graphic, std::vector<glm::vec2>& edges) {
    glm::vec2 bl = glm::vec2(_topLeft.x + c * BLOCK_SIZE, _topLeft.y + (r + 1) * BLOCK_SIZE);
    auto& game = Game::GetInstance();
    auto height = game.getHeight();
    bl.y = height - bl.y;
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
    // 下，上，右，左
    static int dr[4] = { 1, -1, 0, 0 };
    static int dc[4] = { 0, 0, 1, -1 };
    bool filled = tiles[r][c].type == TileType::BRICK;
    if (filled) {

        if (!shadow[r][c] && tiles[r][c].type == TileType::BRICK) {
            game.getGraphics()->drawQuad(bl, glm::vec2(BLOCK_SIZE), colors[tiles[r][c].color]);
        }

        auto curTL = _topLeft + glm::vec2(c * BLOCK_SIZE, r * BLOCK_SIZE);
        for (int i = 0; i < 4; i++) {
            int nr = r + dr[i];
            int nc = c + dc[i];
            if (nr < 0 || nr >= _rows || nc < 0 || nc >= _columns || tiles[nr][nc].type != TileType::BRICK
                || (tiles[nr][nc].type == TileType::BRICK && tiles[nr][nc].color != tiles[r][c].color)) {
                glm::vec2 start = curTL, end = curTL;
                if (i == 0) {
                    start = glm::vec2(curTL.x, curTL.y + BLOCK_SIZE);
                    end = glm::vec2(curTL.x + BLOCK_SIZE, curTL.y + BLOCK_SIZE);
                }
                else if (i == 1) {
                    start = glm::vec2(curTL.x, curTL.y);
                    end = glm::vec2(curTL.x + BLOCK_SIZE, curTL.y);
                }
                else if (i == 2) {
                    start = glm::vec2(curTL.x + BLOCK_SIZE, curTL.y);
                    end = glm::vec2(curTL.x + BLOCK_SIZE, curTL.y + BLOCK_SIZE);
                }
                else {
                    start = glm::vec2(curTL.x, curTL.y);
                    end = glm::vec2(curTL.x, curTL.y + BLOCK_SIZE);
                }
                start.y = height - start.y;
                end.y = height - end.y;
                edges.push_back(start);
                edges.push_back(end);
            }
        }
    }

}
