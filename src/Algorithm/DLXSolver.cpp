#include "DLXSolver.h"
#include <GLFW/glfw3.h>
#include <queue>

std::mutex _mutexLock;

DLXSolver::DLXSolver(int r, int c) :rows(r), cols(c) {
    memset(colLink, 0, sizeof(ColLink) * (c + 1));
    memset(rowLink, 0, sizeof(RowLink) * (r + 1));
    memset(nodes, 0, sizeof(DLXNode) * (r * c + 5));
    for (int i = 0; i <= c; i++) {
        nodes[i].R = i + 1, nodes[i].L = i - 1;
        nodes[i].U = nodes[i].D = i;
        colLink[i].dup = 1;
    }
    nodes[0].L = c;
    nodes[c].R = 0;
    tot = c;

    top = 0;
    found = false, finished = false;
    numSolutions = 0;
}

DLXSolver::~DLXSolver() {
    delete _bruteForce;
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
    if (!rowLink[r].head) {
        rowLink[r].head = p;
        nodes[p].L = nodes[p].R = p;
    }
    else {
        int last = nodes[rowLink[r].head].L;
        nodes[p].R = rowLink[r].head, nodes[p].L = last;
        nodes[last].R = p, nodes[rowLink[r].head].L = p;
    }

}

void DLXSolver::remove(int c) {
    colLink[c].dup--;
    if (colLink[c].dup != 0) return;
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
    ++colLink[c].dup;
    if (colLink[c].dup != 1) return;
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


void DLXSolver::mixSolve(int maxDepthDLX) {
    mixMaxDepth = maxDepthDLX;
    top = 0;
    found = false;
    finished = false;
    auto time = glfwGetTime();
    _bruteForce->startListening();
    _dfsMix(0);
    _bruteForce->notifyComplete();

    printf("%lf seconds\n", glfwGetTime() - time);

    _mutexLock.lock();
    _intermidiateResult.clear();
    for (int i = 0; i < top; i++) _intermidiateResult.push_back(ans[i]);
    _mutexLock.unlock();

    numSolutions = _bruteForce->_numSolutions;
    printf("Finished: %d solutions found\n", numSolutions);
}

void DLXSolver::solve() {
    top = 0;
    found = false;
    finished = false;
    auto time = glfwGetTime();
    _dfs(0);
    printf("%lf seconds\n", glfwGetTime() - time);

    _mutexLock.lock();
    _intermidiateResult.clear();
    for (int i = 0; i < top; i++) _intermidiateResult.push_back(ans[i]);
    _mutexLock.unlock();

    printf("Finished: %d solutions found\n", numSolutions);
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



void DLXSolver::_dfs(int level) {
    if (found) return;
    if (!nodes[0].R) {
        numSolutions++;
        printf("Solution found!\n");
        found = true;
        return;
    }
    int tar = nodes[0].R;
    for (int i = tar; i; i = nodes[i].R) {
        if (colLink[i].sz < colLink[tar].sz) {
            tar = i;
        }
    }

    if (colLink[tar].sz == 0) return;
    remove(tar);
    for (int i = nodes[tar].D; i != tar; i = nodes[i].D) {
        // std::this_thread::sleep_for(std::chrono::milliseconds(20));
        _mutexLock.lock();
        ans[top++] = nodes[i].row;
        _mutexLock.unlock();


        for (int j = nodes[i].R; j != i; j = nodes[j].R) remove(nodes[j].col);
        _dfs(level + 1);
        if (found) return;

        for (int j = nodes[i].L; j != i; j = nodes[j].L) recover(nodes[j].col);


        _mutexLock.lock();
        top--;
        _mutexLock.unlock();
    }
    recover(tar);
}

void DLXSolver::_dfsMix(int dep) {
    if (found) return;
    int tar = nodes[0].R;
    for (int i = tar; i; i = nodes[i].R) {
        if (colLink[i].sz < colLink[tar].sz) {
            tar = i;
        }
    }

    if (colLink[tar].sz == 0) return;

    if (dep == mixMaxDepth) {
        int tar = nodes[0].R;
        int S = 0;
        std::bitset<MAX_SHAPE_SIZE> place = 0;
        place = ~place;
        for (int i = tar; i; i = nodes[i].R) {
            if (i <= numShapes) {
                S |= (1 << (i - 1));
            }
            else {
                place.set(i - numShapes - 1, 0);
            }
        }
        _bruteForce->push(S, place);
        return;
    }

    remove(tar);
    for (int i = nodes[tar].D; i != tar; i = nodes[i].D) {
        // std::this_thread::sleep_for(std::chrono::milliseconds(20));
        _mutexLock.lock();
        ans[top++] = nodes[i].row;
        _mutexLock.unlock();

        for (int j = nodes[i].R; j != i; j = nodes[j].R) remove(nodes[j].col);
        _dfsMix(dep + 1);
        if (found) return;

        for (int j = nodes[i].L; j != i; j = nodes[j].L) recover(nodes[j].col);

        _mutexLock.lock();
        top--;
        _mutexLock.unlock();


    }
    recover(tar);
}
