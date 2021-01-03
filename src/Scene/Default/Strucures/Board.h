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
    std::bitset<128> G;

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
    void gBit();

    std::shared_ptr<Texture2D> generateTexture(glm::vec3 color) const;
};


struct Tile {
    TileType type;
    int color;
};


class Board {
public:
    static constexpr int MAX_BOARD_SIZE = 10;
    static constexpr int BLOCK_SIZE = 32;
    static constexpr int BLOCK_EDGE = 1;

    Board(int rows, int columns, glm::vec2 center);
    void place(const Brick& brick, glm::ivec2 pos, int id, TileType type);

    // 拿走这个位置所代表的砖块，返回砖块的颜色
    int unplace(glm::ivec2 pos);
    void placeShadow(const Brick& brick, glm::ivec2 pos, int color);
    bool canPlace(const Brick& brick, glm::ivec2 pos);
    glm::ivec2 getIndexFromMousePos(const Brick& brick, glm::vec2 pos) const;
    glm::ivec2 getShadowIndexFromMousePos(const Brick& brick, glm::vec2 pos) const;
    bool mouseInside(glm::vec2 mousePos) const;

    bool testCanPlace(const Brick& brick) const;

    void clearShadow();
    void update();
    void draw();
    void remove(const Brick& brick, glm::ivec2 pos);

    int getRows() const { return _rows; }
    int getCols() const { return _columns; }


private:

    Tile tiles[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    int shadow[MAX_BOARD_SIZE][MAX_BOARD_SIZE];
    std::bitset<128> S;

    int _rows, _columns;

    glm::vec2 _topLeft;
    glm::vec2 _center;

    // 查询这一行这一列是哪个砖块，或者-1空气
    int checkID(glm::ivec2 pos);
    void drawCell(int r, int c, std::shared_ptr<Graphics> graphic, std::vector<glm::vec2>& edges);
    void setTile(int r, int c, TileType type, int color);
};
