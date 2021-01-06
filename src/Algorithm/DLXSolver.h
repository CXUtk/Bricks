#pragma once
#include <algorithm>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
struct DLXSolver {
public:
    DLXSolver(int r, int c, int hold);

    void link(int r, int c);
    void remove(int c);
    void recover(int c);
    std::vector<int>  solve();
    std::vector<int> getIntermidiateResult();

private:
    static constexpr int MAX_ROW = 100 * 8 * 105;
    static constexpr int MAX_COL = 250;

    // 双向十字链表
    struct DLXNode {
        int L, R, U, D;
        int row, col;
    };
    struct ColLink {
        int sz;
    };
    struct RowLink {
        int head;
    };
    int rows, cols, tot;
    DLXNode nodes[MAX_ROW * MAX_COL];
    int sz[MAX_COL];
    int head[MAX_ROW];
    int ans[MAX_ROW], top;
    bool found;


    // 如果使用完所有的砖块也可以结束
    int hold;



    int newNode(int r, int c) {
        ++tot;
        nodes[tot].row = r, nodes[tot].col = c;
        return tot;
    }

    void _dfs(int numHold);

};
