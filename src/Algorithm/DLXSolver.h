#pragma once
#include <algorithm>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
struct DLXSolver {
public:
    DLXSolver(int r, int c);

    bool isFinished() const;
    void link(int r, int c);
    void remove(int c);
    void recover(int c);

    void setColDuplicates(int c, int d) { colLink[c].dup = d; }

    void solve();
    std::vector<int> getIntermidiateResult();

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
    DLXNode nodes[MAX_ROW * MAX_COL];
    ColLink colLink[MAX_COL];


    int head[MAX_ROW];
    int ans[MAX_ROW], top;
    bool found, finished;


    std::vector<int> _intermidiateResult;


    int newNode(int r, int c) {
        ++tot;
        nodes[tot].row = r, nodes[tot].col = c;
        return tot;
    }

    void _dfs();

};
