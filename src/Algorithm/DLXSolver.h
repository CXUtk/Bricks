#pragma once
#include <algorithm>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <bitset>
#include "Scene/Default/Strucures/Shape.h"

struct DLXSolver {
public:
    DLXSolver(int r, int c);

    bool isFinished() const;
    void link(int r, int c);
    void remove(int c);
    void recover(int c);

    void setColDuplicates(int c, int d) { colLink[c].dup = d; }
    void setConfigInfo(int num, int rows, int cols) {
        numShapes = num;
        frameRows = rows, frameCols = cols;
    }

    void solve();
    std::vector<int> getIntermidiateResult();
    std::vector<std::pair<int, std::bitset<MAX_SHAPE_SIZE>>> getHalfSolved() const { return _halfSolveRes; }

    int parity[100];

private:
    static constexpr int MAX_ROW = 5005;
    static constexpr int MAX_COL = 512;

    // 双向十字链表
    struct DLXNode {
        int L, R, U, D;
        int row, col;
    };
    struct ColLink {
        int sz, dup;
    };
    struct RowLink {
        int head;
    };


    int rows, cols, tot;
    int frameRows, frameCols;
    int numShapes;
    DLXNode nodes[MAX_ROW * MAX_COL];
    ColLink colLink[MAX_COL];
    RowLink rowLink[MAX_ROW];



    int ans[MAX_ROW], top;
    bool found, finished;
    int numSolutions;


    std::vector<int> _intermidiateResult;
    std::vector<std::pair<int, std::bitset<MAX_SHAPE_SIZE>>> _halfSolveRes;



    int newNode(int r, int c) {
        ++tot;
        nodes[tot].row = r, nodes[tot].col = c;
        return tot;
    }

    void _dfs(int dep);
    int _bfs(int x, std::bitset<MAX_SHAPE_SIZE>& mapp);
};
