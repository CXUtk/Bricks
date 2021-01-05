#include "DLXSolver.h"

DLXSolver::DLXSolver(int r, int c, int hold) :rows(r), cols(c), hold(hold) {
    memset(colLink, 0, sizeof(ColLink) * (c + 1));
    memset(rowLink, 0, sizeof(RowLink) * (r + 1));
    memset(nodes, 0, sizeof(DLXNode) * (r * c + 5));
    for (int i = 0; i <= c; i++) {
        nodes[i].R = i + 1, nodes[i].L = i - 1;
        nodes[i].U = nodes[i].D = i;
    }
    nodes[0].L = c;
    nodes[c].R = 0;
    tot = c;

    top = 0;
    found = false;
}

void DLXSolver::link(int r, int c) {
    int p = newNode(r, c), down = nodes[c].D;
    colLink[c].sz++;

    nodes[p].U = c, nodes[p].D = down;
    // 把p链接在列链表的头部
    nodes[down].U = p, nodes[c].D = p;

    // 行链表加入这个节点
    if (!rowLink[r].head) {
        nodes[p].L = nodes[p].R = p;
    }
    else {
        int last = rowLink[r].head;
        // 把p加入行链表的头部
        nodes[nodes[last].L].R = p;
        nodes[p].L = nodes[last].L;
        nodes[last].L = p;
        nodes[p].R = last;
    }
    rowLink[r].head = p;
}

void DLXSolver::remove(int c) {
    // 列链表的头部移除
    nodes[nodes[c].L].R = nodes[c].R;
    nodes[nodes[c].R].L = nodes[c].L;
    // 把占有这一列的所有行删除
    for (int i = nodes[c].D; i != c; i = nodes[i].D) {
        for (int j = nodes[i].R; j != i; j = nodes[j].R) {
            // 删除其垂直链表，但是水平链表不动
            nodes[nodes[j].U].D = nodes[j].D;
            nodes[nodes[j].D].U = nodes[j].U;
            colLink[c].sz--;
        }
    }
}

void DLXSolver::recover(int c) {
    // 把占有这一列的所有行恢复
    for (int i = nodes[c].U; i != c; i = nodes[i].U) {
        for (int j = nodes[i].L; j != i; j = nodes[j].L) {
            // 恢复垂直链表
            nodes[nodes[j].U].D = j;
            nodes[nodes[j].D].U = j;
            colLink[c].sz++;
        }
    }
    nodes[nodes[c].L].R = c;
    nodes[nodes[c].R].L = c;
}

std::vector<int> DLXSolver::solve() {
    top = 0;
    found = false;
    _dfs();
    std::vector<int> res;
    for (int i = 0; i < top; i++) res.push_back(ans[i]);
    return res;
}

void DLXSolver::_dfs() {
    if (found) return;
    if (!nodes[0].R || nodes[0].R > hold) {
        found = true;
        return;
    }
    int tar = nodes[0].R;
    for (int i = tar; i; i = nodes[i].R) {
        if (colLink[i].sz < colLink[tar].sz) {
            tar = i;
        }
    }
    remove(tar);
    for (int i = nodes[tar].D; i != tar; i = nodes[i].D) {
        ans[top++] = nodes[i].row;
        for (int j = nodes[i].R; j != i; j = nodes[j].R) remove(nodes[j].col);
        _dfs();
        if (found) return;
        for (int j = nodes[i].L; j != i; j = nodes[j].L) recover(nodes[j].col);
        top--;
    }
    recover(tar);
}
