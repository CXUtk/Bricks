
#include <mutex>
#include "BruteForce.h"

int BruteForce::_numSolutions = 0;
static std::mutex taskQueueLock;
static std::mutex resultLock;

BruteForce::BruteForce(int frameRows, int frameCols, int shapeNumber) :
    _rows(frameRows), _cols(frameCols), _shapeNums(shapeNumber) {
    _numSolutions = 0;
    _complete = false;
}

BruteForce::~BruteForce() {
    for (auto th : _threads) {
        delete th;
    }
}

void BruteForce::push(int S, std::bitset<MAX_SHAPE_SIZE> P) {
    taskQueueLock.lock();
    _taskQueue.push({ S, P });
    taskQueueLock.unlock();

}

static int numThreads = 10;
void BruteForce::startListening() {

    for (int i = 0; i < numThreads; i++) {
        _threads.push_back(new std::thread([&]() {
            while (true) {
                std::vector<std::pair<int, std::bitset<MAX_SHAPE_SIZE>>> config;

                bool empty = false;
                taskQueueLock.lock();
                if (!_taskQueue.empty()) {
                    int c = 0;
                    while (c < 1 && !_taskQueue.empty()) {
                        config.push_back(_taskQueue.front());
                        _taskQueue.pop();
                        c++;
                    }
                }
                else {
                    empty = true;
                }
                taskQueueLock.unlock();
                if (empty) {
                    if (_complete)break;
                    continue;
                }


                int num = 0;
                for (auto& conf : config) {
                    _dfs(conf.first, 0, conf.second, num);
                }

                resultLock.lock();
                _numSolutions += num;
                printf("%d\n", num);
                resultLock.unlock();

            }
            }));
    }

}

void BruteForce::notifyComplete() {
    _complete = true;
    for (int i = 0; i < numThreads; i++) {
        _threads[i]->join();
    }
}

void BruteForce::_dfs(int S, int start, std::bitset<MAX_SHAPE_SIZE>& P, int& numSolutions) {
    if (!S) {
        numSolutions++;
        // found = true;
        return;
    }
    int sz = _shapeNums;
    int minj = -1;

    int cnt = 0;
    for (int j = start; j < _rows * _cols; j++) {
        if (!P[j]) {
            if (minj == -1) minj = j;

            if (j != 0 && P[j - 1] && j + _cols < _rows * _cols && P[j + _cols] && j - _cols >= 0 && P[j - _cols] && j != _rows * _cols - 1 && P[j + 1]) return;
        }
    }
    int tR = minj / _cols;
    int tC = minj % _cols;
    std::bitset<MAX_SHAPE_SIZE> M = P >> (tR * _cols + tC);
    for (int i = 0; i < sz; i++) {
        if (S >> i & 1) {
            for (const auto& p : _bruteForceInfo[i]) {
                if (tR >= _rows - p.r + 1) continue;
                if (tC - p.xoffset < 0 || tC - p.xoffset >= _cols - p.c + 1) continue;

                // 如果放置不能匹配
                if (((M << p.xoffset) & p.bits).any()) continue;
                auto tmp = p.bits << (minj - p.xoffset);
                P |= tmp;
                //Ans.push_back(i);
                _dfs(S ^ (1 << i), minj + 1, P, numSolutions);
                // Ans.pop_back();
                P ^= tmp;
            }
        }
    }
}
