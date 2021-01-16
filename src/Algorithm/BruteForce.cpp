#include "BruteForce.h"

BruteForce::BruteForce(int frameRows, int frameCols, int shapeNumber) :
    _rows(frameRows), _cols(frameCols), _shapeNums(shapeNumber) {
    _numSolutions = 0;
}

void BruteForce::run(int S, std::bitset<MAX_SHAPE_SIZE> P) {
    _bruteFrameState = P;
    _dfs(S, 0);
}

void BruteForce::_dfs(int S, int start) {
    if (!S) {
        _numSolutions++;
        if (_numSolutions % 30 == 0) {
            printf("%d\n", _numSolutions);
        }
        // found = true;
        return;
    }
    int sz = _shapeNums;
    int tR = -1, tC = -1;
    for (int j = start; j < _rows * _cols; j++) {
        if (!_bruteFrameState[j]) {
            tR = j / _cols;
            tC = j % _cols;
            break;
        }
    }
    for (int i = 0; i < sz; i++) {
        if (S >> i & 1) {
            for (auto& p : _bruteForceInfo[i]) {
                if (tR >= _rows - p.r + 1) continue;
                if (tC - p.xoffset < 0 || tC - p.xoffset >= _cols - p.c + 1) continue;
                int id2 = tR * _cols + tC - p.xoffset;
                // 如果放置不能匹配
                auto tmp = (p.bits << id2);
                if ((_bruteFrameState & tmp) != 0) continue;

                _bruteFrameState |= tmp;
                //Ans.push_back(i);
                _dfs(S ^ (1 << i), tR * _cols + tC + 1);
                // Ans.pop_back();
                _bruteFrameState ^= tmp;
            }
        }
    }
}
