#include "DLXSolver.h"

DLXSolver::DLXSolver(int r, int c) :rows(r), cols(c) {
    memset(rowLink, 0, sizeof(RowLink) * (r + 1));
    memset(nodes, 0, sizeof(DLXNode) * (r * c + 5));
}

void DLXSolver::link(int r, int c) {
}

void DLXSolver::remove(int c) {
}

void DLXSolver::recover(int c) {
}

void DLXSolver::solve() {
}
