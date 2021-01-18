#pragma once
#include <bitset>
#include <vector>
#include <queue>
#include <thread>
#include "Scene/Default/Strucures/Shape.h"
struct ShapeBruteInfo {
    int xoffset;
    int r, c;
    std::bitset<MAX_SHAPE_SIZE> bits;
};

class BruteForce {
public:

    BruteForce() {}
    ~BruteForce();

    BruteForce(int frameRows, int frameCols, int shapeNumber);
    void push(int S, std::bitset<MAX_SHAPE_SIZE> P);
    void startListening();
    void notifyComplete();
    void reset();

    std::vector<ShapeBruteInfo> _bruteForceInfo[1005];
    static int _numSolutions;
    bool _complete;


    void _dfs(std::vector<int>& cnt, int start, std::bitset<MAX_SHAPE_SIZE>& P, int& numSolutions);
private:

    int _rows, _cols, _shapeNums;
    bool _found;

    std::queue<std::pair<int, std::bitset<MAX_SHAPE_SIZE>>> _taskQueue;
    std::vector<std::thread*> _threads;

    void _dfs(int S, int start, std::bitset<MAX_SHAPE_SIZE>& P, int& numSolutions);


};
