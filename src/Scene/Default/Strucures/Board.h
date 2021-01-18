#pragma once
#include <glm/glm.hpp>
#include <bitset>
#include <memory>
#include "Render/Graphic.h"
#include "Shape.h"


class Board {
public:
    static constexpr int BLOCK_SIZE = 32;
    static constexpr int BLOCK_EDGE = 1;

    Board(int rows, int columns);
    void place(const Shape& brick, glm::ivec2 pos, int id);

    // 拿走这个位置所代表的砖块，返回砖块的覆盖掩码
    std::bitset<MAX_SHAPE_SIZE> unplace(glm::ivec2 pos, int& id);

    void placeShadow(const Shape& brick, glm::ivec2 pos, int color);
    bool canPlace(const Shape& brick, glm::ivec2 pos);
    glm::ivec2 getIndexFromMousePos(const Shape& brick, glm::vec2 pos) const;
    glm::ivec2 getShadowIndexFromMousePos(const Shape& brick, glm::vec2 pos) const;
    bool mouseInside(glm::vec2 mousePos) const;

    void clearShadow();
    void update(glm::ivec2 topLeft);
    void draw();
    void remove(const Shape& brick, glm::ivec2 pos);
    void clear(const std::bitset<MAX_SHAPE_SIZE>& mask);

    int getRows() const { return _rows; }
    int getCols() const { return _columns; }

    glm::ivec2 getSize() const;


private:
    struct Tile {
        int id, color, shadow;
    };
    Tile* tiles;

    int _rows, _columns;
    glm::ivec2 _topLeft;


    void drawCell(int r, int c, std::shared_ptr<Graphics> graphic);
    void setTile(int r, int c, int k, int color);

    int getTileID(int r, int c) const;
    int getTileColor(int r, int c) const;
    int getShadow(int r, int c)const;
};
