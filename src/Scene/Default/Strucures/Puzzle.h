#pragma once

#include <glm/glm.hpp>
#include <bitset>
#include <memory>

static constexpr int MAX_SHAPE_SIZE = 256;

struct Shape {
    std::bitset<MAX_SHAPE_SIZE> _bits;
};

class Puzzel {
    // 每个拼图块的最大占据格子数量

public:

private:
};
