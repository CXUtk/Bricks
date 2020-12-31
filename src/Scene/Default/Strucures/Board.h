#pragma once
#include <glm/glm.hpp>

enum class TileType {
    EMPTY,
    BRICK
};

struct Brick {
    static constexpr int MAX_SIZE = 30;

    int n, m;
    int S[MAX_SIZE];

    Brick() = default;
    Brick(int n, int m);
    Brick(int n, int m, const char* shape);
    Brick(int n, int m, const int* shape);

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
    Board(int rows, int columns);
    void place(const Brick& brick, glm::ivec2 pos, int id, TileType type);
    void placeShadow(const Brick& brick, glm::ivec2 pos);
    bool canPlace(const Brick& brick, glm::ivec2 pos);
    glm::ivec2 getIndexFromPos(glm::vec2 pos) const;
    void clearShadow();
    void update();
    void draw();
    void remove(const Brick& brick, glm::ivec2 pos);

    int getRows() const { return _rows; }
    int getCols() const { return _columns; }

private:
    static constexpr int MAX_SIZE = 10;
    static constexpr int BLOCK_SIZE = 32;
    static constexpr int BLOCK_EDGE = 1;
    Tile tiles[MAX_SIZE][MAX_SIZE];
    bool shadow[MAX_SIZE][MAX_SIZE];

    int _rows, _columns;

    glm::vec2 _topLeft;
};
