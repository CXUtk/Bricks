#pragma once

#include <glm/glm.hpp>
#include <bitset>
#include <memory>
#include <vector>
#include "Algorithm/DLXSolver.h"

// 每个拼图块的最大占据格子数量
static constexpr int MAX_SHAPE_SIZE = 256;

struct Shape {
    int rows, cols;
    std::bitset<MAX_SHAPE_SIZE> bits;

    Shape() :rows(0), cols(0) {}
    Shape(int rows, int cols) :rows(rows), cols(cols), bits(0) {}

    Shape rotateClockwise() const;
    Shape flip() const;

    bool operator<(const Shape& shape) const {
        auto p1 = std::make_pair(rows, cols);
        auto p2 = std::make_pair(shape.rows, shape.cols);
        return p1 < p2 || (p1 == p2 && bits.to_string() < shape.bits.to_string());
    }

    bool operator==(const Shape& shape) const {
        auto p1 = std::make_pair(rows, cols);
        auto p2 = std::make_pair(shape.rows, shape.cols);
        return p1 == p2 && bits == shape.bits;
    }

    int count() const;
};


struct Shape_Info {
    int id;
    int r, c, s;
    Shape_Info() = default;
    Shape_Info(int id, int r, int c, int s) : id(id), r(r), c(c), s(s) {}
};

class Puzzel {
public:
    Puzzel();
    ~Puzzel();

    void setFrameSize(int r, int c);
    void add(const Shape& shape);
    void build();
    void solve();

private:

    int _rows, _cols;

    std::vector<Shape> _shapes;
    std::vector<int> _shapeDups;
    std::vector<Shape_Info> _shapeInfo;
    // Shape _mainFrame;
    DLXSolver* _solver;


    // 判重，移除相同块，有下面那个还要这个干嘛？？
    void remove_dupI();
    // 判重，移除本质相同块
    void remove_dupStrongI();


    void init_dlx();

    int getID(int r, int c) const { return r * _cols + c; }
};
