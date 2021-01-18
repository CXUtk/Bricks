#pragma once
#include <glm/glm.hpp>
#include <bitset>
#include <memory>
#include <vector>
#include <Render\Texture2D.h>

// 每个拼图块的最大占据格子数量
static constexpr int MAX_SHAPE_SIZE = 144;

struct Shape {
    int rows, cols;
    std::bitset<MAX_SHAPE_SIZE> bits;

    Shape() :rows(0), cols(0) {}
    Shape(int rows, int cols) :rows(rows), cols(cols), bits(0) {}
    Shape(int rows, int cols, const char* shape);

    // 获取顺时针旋转90度的物块
    Shape rotateClockwise() const;
    // 获取翻转后的物块
    Shape flip() const;

    bool get(int i, int j) const { return bits.test(i * cols + j); }

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

    // 计算有多少个实心块
    int count() const;

    std::shared_ptr<Texture2D> generateTexture(glm::vec3 color) const;

    int getTopLeftFittingXOffset() const;
    std::bitset<MAX_SHAPE_SIZE> gBit(int frameCols) const;
};
