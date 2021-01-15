#include "DLXSolver.h"
#include <Scene\Default\Strucures\Board.h>

std::mutex _mutexLock;

DLXSolver::DLXSolver(int r, int c) :rows(r), cols(c) {
    memset(colLink, 0, sizeof(ColLink) * (c + 1));
    memset(head, 0, sizeof(RowLink) * (r + 1));
    memset(nodes, 0, sizeof(DLXNode) * (r * c + 5));
    for (int i = 0; i <= c; i++) {
        nodes[i].R = i + 1, nodes[i].L = i - 1;
        nodes[i].U = nodes[i].D = i;
    }
    nodes[0].L = c;
    nodes[c].R = 0;
    tot = c;

    top = 0;
    found = false, finished = false;
}

bool DLXSolver::isFinished() const {
    bool f;
    _mutexLock.lock();
    f = finished;
    _mutexLock.unlock();
    return f;
}

void DLXSolver::link(int r, int c) {
    int p = newNode(r, c), down = nodes[c].D;
    colLink[c].sz++;

    nodes[p].U = c, nodes[p].D = down;
    // 把p链接在列链表的头部
    nodes[down].U = nodes[c].D = p;

    // 行链表加入这个节点
    if (!head[r]) {
        head[r] = p;
        nodes[p].L = nodes[p].R = p;
    }
    else {
        int last = nodes[head[r]].L;
        nodes[p].R = head[r], nodes[p].L = last;
        nodes[last].R = p, nodes[head[r]].L = p;
    }

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
            colLink[nodes[j].col].sz--;
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
            colLink[nodes[j].col].sz++;
        }
    }
    nodes[nodes[c].L].R = c;
    nodes[nodes[c].R].L = c;
}

void DLXSolver::solve() {
    top = 0;
    found = false;
    finished = false;
    _dfs();
    _intermidiateResult.clear();
    for (int i = 0; i < top; i++) _intermidiateResult.push_back(ans[i]);
    _mutexLock.lock();
    finished = true;
    _mutexLock.unlock();
    printf("Finished\n");
}

std::vector<int> DLXSolver::getIntermidiateResult() {
    std::vector<int> res;
    _mutexLock.lock();
    for (int i = 0; i < top; i++) {
        res.push_back(ans[i]);
    }
    _mutexLock.unlock();
    return res;
}

void DLXSolver::_dfs() {
    if (found) return;
    if (!nodes[0].R) {
        found = true;
        printf("Solution Found!\n");
        return;
    }

    int tar = nodes[0].R;
    int cnt = 0;
    for (int i = tar; i; i = nodes[i].R) {
        if (colLink[i].sz < colLink[tar].sz) {
            tar = i;
        }
    }

    remove(tar);

    for (int i = nodes[tar].D; i != tar; i = nodes[i].D) {
        //std::this_thread::sleep_for(std::chrono::milliseconds(200));
        ans[top++] = nodes[i].row;
        for (int j = nodes[i].R; j != i; j = nodes[j].R) remove(nodes[j].col);
        _dfs();
        if (found) return;
        top--;
        for (int j = nodes[i].L; j != i; j = nodes[j].L) recover(nodes[j].col);
    }
    recover(tar);
}
