#pragma once
#include <algorithm>
#include <memory>
struct DLXSolver {
public:
    DLXSolver(int r, int c);

    void link(int r, int c);
    void remove(int c);
    void recover(int c);
    void solve();

private:
    static constexpr int MAX_ROW = 100 * 8 * 100 * 2;
    static constexpr int MAX_COL = 100;
    struct DLXNode {
        int L, R, U, D;
        int row, col;
    };
    struct RowLink {
        int head, sz;
    };
    int rows, cols, tot;
    DLXNode nodes[MAX_ROW * MAX_COL];
    RowLink rowLink[MAX_ROW];
    int ans[MAX_ROW];


    int newNode(int r, int c) {
        ++tot;
        nodes[tot].row = r, nodes[tot].col = c;
        return tot;
    }

};
