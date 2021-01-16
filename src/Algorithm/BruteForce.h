#pragma once
#include <bitset>
#include <vector>
#include "Scene/Default/Strucures/Shape.h"
struct ShapeBruteInfo {
    int xoffset;
    int r, c;
    std::bitset<MAX_SHAPE_SIZE> bits;
};

class BruteForce {
public:

    BruteForce() {}
    BruteForce(int frameRows, int frameCols, int shapeNumber);
    void run(int S, std::bitset<MAX_SHAPE_SIZE> P);

    std::vector<ShapeBruteInfo> _bruteForceInfo[1005];
    int _numSolutions;
private:

    std::bitset<MAX_SHAPE_SIZE> _bruteFrameState;
    int _rows, _cols, _shapeNums;
    void _dfs(int S, int start);
};
