#pragma once
#include <algorithm>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <bitset>
#include "Scene/Default/Strucures/Shape.h"
#include "BruteForce.h"

struct DLXSolver {
public:
    DLXSolver(int r, int c);
    ~DLXSolver();

    bool isFinished() const;
    void link(int r, int c);
    void remove(int c);
    void recover(int c);

    void setColDuplicates(int c, int d) { colLink[c].dup = d; }
    void setConfigInfo(int num, int rows, int cols) {
        numShapes = num;
        frameRows = rows, frameCols = cols;
        _bruteForce = new BruteForce(frameRows, frameCols, numShapes + 1);
    }

    void addBruteInfo(int i, ShapeBruteInfo info) {
        _bruteForce->_bruteForceInfo[i].push_back(info);
    }
    void solve_brute(std::vector<int>& cnt, int start, std::bitset<MAX_SHAPE_SIZE>& P, int& numSolutions);

    void mixSolve(int maxDepthDLX);
    void solve();
    std::vector<int> getIntermidiateResult();
    bool found, finished;

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

    int numSolutions;
    int mixMaxDepth;


    std::vector<int> _intermidiateResult;
    BruteForce* _bruteForce;



    int newNode(int r, int c) {
        ++tot;
        nodes[tot].row = r, nodes[tot].col = c;
        return tot;
    }

    void _dfs(int dep);
    void _dfsMix(int dep);
};
