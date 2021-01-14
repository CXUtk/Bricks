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

    bool operator==(const Brick& other) const {
        return n == other.n && m == other.m && S == other.S;
    }

    bool operator<(const Brick& other) const {
        return S.to_string() < other.S.to_string();
    }

    size_t operator()(const Brick& b) const noexcept {
        auto fn = std::hash<std::bitset<128>>();
        return fn(b.S) + n + m;
    }

    bool get(int r, int c) const {
        return S.test(r * m + c);
    }

    // Generator
    Brick flip() const;
    Brick rotateClockwise() const;
    std::bitset<128> gBit(int colNum) const;

    std::shared_ptr<Texture2D> generateTexture(glm::vec3 color) const;
};




class Board {
public:
    static constexpr int BLOCK_SIZE = 32;
    static constexpr int BLOCK_EDGE = 1;

    Board(int rows, int columns);
    void place(const Brick& brick, glm::ivec2 pos, int id);

    // 拿走这个位置所代表的砖块，返回砖块的颜色
    int unplace(glm::ivec2 pos);
    void placeShadow(const Brick& brick, glm::ivec2 pos, int color);
    bool canPlace(const Brick& brick, glm::ivec2 pos);
    glm::ivec2 getIndexFromMousePos(const Brick& brick, glm::vec2 pos) const;
    glm::ivec2 getShadowIndexFromMousePos(const Brick& brick, glm::vec2 pos) const;
    bool mouseInside(glm::vec2 mousePos) const;

    void clearShadow();
    void update(glm::ivec2 topLeft);
    void draw();
    void remove(const Brick& brick, glm::ivec2 pos);
    void clear();

    int getRows() const { return _rows; }
    int getCols() const { return _columns; }

    glm::ivec2 getSize() const;


private:

    int* tiles;
    int* shadow;

    int _rows, _columns;
    glm::ivec2 _topLeft;


    void drawCell(int r, int c, std::shared_ptr<Graphics> graphic);
    void setTile(int r, int c, int color);

    int getTileColor(int r, int c) const;
    int getShadow(int r, int c)const;


};
