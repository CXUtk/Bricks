#include "Board.h"
#include "Game.h"
#include <algorithm>

Board::Board(int rows, int columns) :_rows(rows), _columns(columns) {
    memset(tiles, 0, sizeof(tiles));
    memset(shadow, 0, sizeof(shadow));
}

void Board::place(const Brick& brick, glm::ivec2 pos, int id, TileType type) {
    for (int i = pos.x; i < std::min(pos.x + brick.n, MAX_SIZE); i++) {
        for (int j = pos.y; j < std::min(pos.y + brick.m, MAX_SIZE); j++) {
            int r = i - pos.x, c = j - pos.y;
            if (brick.S[r] >> c & 1) {
                tiles[i][j].type = type;
                tiles[i][j].color = id;
            }
        }
    }
}

void Board::placeShadow(const Brick& brick, glm::ivec2 pos) {
    for (int i = pos.x; i < std::min(pos.x + brick.n, MAX_SIZE); i++) {
        for (int j = pos.y; j < std::min(pos.y + brick.m, MAX_SIZE); j++) {
            int r = i - pos.x, c = j - pos.y;
            if (brick.S[r] >> c & 1) {
                shadow[i][j] = true;
            }
        }
    }
}


bool Board::canPlace(const Brick& brick, glm::ivec2 pos) {
    if (pos.x + brick.n > MAX_SIZE || pos.y + brick.m > MAX_SIZE) return false;
    for (int i = pos.x; i < std::min(pos.x + brick.n, MAX_SIZE); i++) {
        for (int j = pos.y; j < std::min(pos.y + brick.m, MAX_SIZE); j++) {
            int r = i - pos.x, c = j - pos.y;
            if ((brick.S[r] >> c & 1) && tiles[i][j].type != TileType::EMPTY) {
                return false;
            }
        }
    }
    return true;
}

glm::ivec2 Board::getIndexFromPos(glm::vec2 pos) const {
    pos -= _topLeft;
    return glm::ivec2(std::max(0, std::min(MAX_SIZE - 1, (int)(pos.y / BLOCK_SIZE))), std::max(0, std::min(MAX_SIZE - 1, (int)(pos.x / BLOCK_SIZE))));
}

void Board::clearShadow() {
    memset(shadow, 0, sizeof(shadow));
}

void Board::update() {
    auto& game = Game::GetInstance();
    glm::vec2 center = glm::vec2(game.getWidth(), game.getHeight()) * 0.5f;
    _topLeft = center - glm::vec2(_rows * 0.5f * BLOCK_SIZE, _columns * 0.5f * BLOCK_SIZE);
}

void Board::draw() {
    auto& game = Game::GetInstance();
    std::vector<glm::vec2> lines;
    for (int i = 0; i <= _rows; i++) {
        glm::vec2 start = glm::vec2(_topLeft.x, _topLeft.y + i * BLOCK_SIZE);
        start.y = game.getHeight() - start.y;
        glm::vec2 end = glm::vec2(_topLeft.x + _columns * BLOCK_SIZE, _topLeft.y + i * BLOCK_SIZE);
        end.y = game.getHeight() - end.y;

        lines.push_back(start);
        lines.push_back(end);
    }

    for (int i = 0; i <= _columns; i++) {
        glm::vec2 start = glm::vec2(_topLeft.x + i * BLOCK_SIZE, _topLeft.y);
        start.y = game.getHeight() - start.y;
        glm::vec2 end = glm::vec2(_topLeft.x + i * BLOCK_SIZE, _topLeft.y + _rows * BLOCK_SIZE);
        end.y = game.getHeight() - end.y;

        lines.push_back(start);
        lines.push_back(end);
    }

    game.getGraphics()->drawLines(lines, glm::vec3(0, 0, 0), 1);

    glm::vec3 colors[] = {
        glm::vec3(1, 0, 1),
        glm::vec3(1, 0, 0),
        glm::vec3(1, 0.8, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 1),
        glm::vec3(0, 1, 1),
        glm::vec3(0, 1, 1),
    };
    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _columns; j++) {
            glm::vec2 bl = glm::vec2(_topLeft.x + j * BLOCK_SIZE, _topLeft.y + (i + 1) * BLOCK_SIZE);
            bl.y = game.getHeight() - bl.y;
            if (shadow[i][j]) {
                game.getGraphics()->drawQuad(bl, glm::vec2(BLOCK_SIZE), glm::vec3(1, 0, 0));
            }
            else if (tiles[i][j].type == TileType::BRICK) {
                game.getGraphics()->drawQuad(bl, glm::vec2(BLOCK_SIZE), colors[tiles[i][j].color]);
            }
        }
    }
}

void Board::remove(const Brick& brick, glm::ivec2 pos) {
    for (int i = pos.x; i < std::min(pos.x + brick.n, MAX_SIZE); i++) {
        for (int j = pos.y; j < std::min(pos.y + brick.m, MAX_SIZE); j++) {
            int r = i - pos.x, c = j - pos.y;
            if (brick.S[r] >> c & 1) {
                tiles[i][j].type = TileType::EMPTY;
            }
        }
    }
}


Brick::Brick(int n, int m) : n(n), m(m) {
}

Brick::Brick(int n, int m, const char* shape) : n(n), m(m) {
    for (int i = 0; i < n; i++) {
        S[i] = 0;
        for (int j = 0; j < m; j++) {
            int id = i * m + j;
            if (shape[id] == 'O') {
                S[i] |= (1 << j);
            }
        }
    }
}

Brick::Brick(int n, int m, const int* shape) : n(n), m(m) {
    for (int i = 0; i < n; i++) {
        S[i] = shape[i];
    }
}

Brick Brick::flip() const {
    Brick out(n, m);
    for (int i = 0; i < n; i++) {
        out.S[i] = 0;
        for (int j = 0; j < m; j++) {
            if (S[i] >> j & 1)
                out.S[i] |= (1 << (m - j - 1));
        }
    }
    return out;
}

Brick Brick::rotateClockwise() const {
    Brick out(m, n);
    for (int i = 0; i < m; i++) out.S[i] = 0;

    for (int i = n - 1; i >= 0; i--) {
        for (int j = 0; j < m; j++) {
            if (S[i] >> j & 1) {
                int k = n - i - 1;
                out.S[j] |= (1 << k);
            }
        }
    }
    return out;
}
