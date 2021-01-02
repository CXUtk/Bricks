﻿#include "Board.h"
#include "Game.h"
#include <algorithm>
#include <glm/gtx/transform.hpp>

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



std::shared_ptr<Texture2D> Brick::generateTexture(glm::vec3 color) const {

    constexpr int BLOCK_SIZE_DRAW = 32;
    auto& game = Game::GetInstance();

    // 生成一个临时framebuffer，然后绘制物块，最后把framebuffer保存为纹理
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // 绑定纹理
    unsigned int texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    int width = m * BLOCK_SIZE_DRAW;
    int height = n * BLOCK_SIZE_DRAW;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m * BLOCK_SIZE_DRAW, n * BLOCK_SIZE_DRAW, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");

    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    auto savedProj = game.getGraphics()->getProjectionMatrix();
    game.getGraphics()->setProjectionMatrix(glm::ortho(0.f, (float)m * BLOCK_SIZE_DRAW, 0.f, (float)n * BLOCK_SIZE_DRAW, -1.0f, 1.0f));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            int id = i * m + j;
            if (S.test(id)) {
                auto pos = glm::vec2(j * BLOCK_SIZE_DRAW, (i + 1) * BLOCK_SIZE_DRAW);
                pos.y = n * BLOCK_SIZE_DRAW - pos.y;
                game.getGraphics()->drawQuad(pos, glm::vec2(BLOCK_SIZE_DRAW), color);
            }
        }
    }

    // 解除绑定
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);
    glViewport(0, 0, game.getWidth(), game.getHeight());
    game.getGraphics()->setProjectionMatrix(savedProj);

    return std::shared_ptr<Texture2D>(new Texture2D(texID, glm::ivec2(m * BLOCK_SIZE_DRAW, n * BLOCK_SIZE_DRAW)));
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

glm::ivec2 Board::getIndexFromPos(const Brick& brick, glm::vec2 pos) const {
    auto halfSize = glm::vec2(brick.m, brick.n) * 16.f;
    pos -= glm::vec2(_topLeft.x + halfSize.x, _topLeft.y + halfSize.y);

    int r = std::max(0, std::min(MAX_BOARD_SIZE - 1, (int)(pos.y / BLOCK_SIZE)));
    r = std::min(r, MAX_BOARD_SIZE - brick.n);
    int c = std::max(0, std::min(MAX_BOARD_SIZE - 1, (int)(pos.x / BLOCK_SIZE)));
    c = std::min(c, MAX_BOARD_SIZE - brick.m);
    return glm::ivec2(r, c);
}

bool Board::mouseInside(glm::vec2 mousePos) const {
    return mousePos.x >= _topLeft.x && mousePos.x <= _topLeft.x + MAX_BOARD_SIZE * BLOCK_SIZE
        && mousePos.y >= _topLeft.y && mousePos.y <= _topLeft.y + MAX_BOARD_SIZE * BLOCK_SIZE;
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
    game.getGraphics()->setProjectionMatrix(glm::ortho(0.f, (float)game.getWidth(), 0.f, (float)game.getHeight(), -1.0f, 1.0f));

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
                auto color = (shadow[i][j] == 1) ? glm::vec3(0.5, 1, 0.5) : glm::vec3(1, 0.5, 0.5);
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
                /*|| (tiles[nr][nc].type == TileType::BRICK && tiles[nr][nc].color != tiles[r][c].color)*/) {
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