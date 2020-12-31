#pragma once
#include <glm/glm.hpp>
#include <bitset>
#include <memory>
#include "Render/Graphic.h"

enum class TileType {
    EMPTY,
    BRICK
};

struct Brick {
    int n, m;
    std::bitset<128> S;

    Brick() = default;
    Brick(int n, int m);
    Brick(int n, int m, const char* shape);
    Brick(int n, int m, std::bitset<128> set);
    bool get(int r, int c) const {
        return S.test(r * m + c);
    }

    // Generator
    Brick flip() const;
    Brick rotateClockwise() const;
};


struct Tile {
    TileType type;
    int color;
};


class Board {
public:
    Board(int rows, int columns, glm::vec2 center);
    void place(const Brick& brick, glm::ivec2 pos, int id, TileType type);
    void placeShadow(const Brick& brick, glm::ivec2 pos, int color);
    bool canPlace(const Brick& brick, glm::ivec2 pos);
    glm::ivec2 getIndexFromPos(glm::vec2 pos) const;
    void clearShadow();
    void update();
    void draw();
    void remove(const Brick& brick, glm::ivec2 pos);

    int getRows() const { return _rows; }
    int getCols() const { return _columns; }

private:
    static constexpr int MAX_BOARD_SIZE = 10;
    static constexpr int BLOCK_SIZE = 32;
    static constexpr int BLOCK_EDGE = 1;
    Tile tiles[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int shadow[MAX_BOARD_SIZE][MAX_BOARD_SIZE];

    int _rows, _columns;

    glm::vec2 _topLeft;
    glm::vec2 _center;


    void drawCell(int r, int c, std::shared_ptr<Graphics> graphic, std::vector<glm::vec2>& edges);
};
