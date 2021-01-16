
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

void BruteForce::startListening() {
    int numThreads = 4;
    for (int i = 0; i < numThreads; i++) {
        _threads.push_back(new std::thread([&]() {
            while (!_complete) {
                std::pair<int, std::bitset<MAX_SHAPE_SIZE>> config;

                bool empty = false;
                taskQueueLock.lock();
                if (!_taskQueue.empty()) {
                    config = _taskQueue.front();
                    _taskQueue.pop();
                }
                else {
                    empty = true;
                }
                taskQueueLock.unlock();
                if (empty) break;


                int num = 0;
                _dfs(config.first, 0, config.second, num);

                resultLock.lock();
                _numSolutions += num;
                printf("%d\n", num);
                resultLock.unlock();

            }
            }));
    }
    for (int i = 0; i < numThreads; i++) {
        _threads[i]->join();
    }
}

void BruteForce::notifyComplete() {
    _complete = true;
}

void BruteForce::_dfs(int S, int start, std::bitset<MAX_SHAPE_SIZE>& P, int& numSolutions) {
    if (!S) {
        numSolutions++;
        // found = true;
        return;
    }
    int sz = _shapeNums;
    int tR = -1, tC = -1;
    for (int j = start; j < _rows * _cols; j++) {
        if (!P[j]) {
            tR = j / _cols;
            tC = j % _cols;
            break;
        }
    }
    for (int i = 0; i < sz; i++) {
        if (S >> i & 1) {
            for (const auto& p : _bruteForceInfo[i]) {
                if (tR >= _rows - p.r + 1) continue;
                if (tC - p.xoffset < 0 || tC - p.xoffset >= _cols - p.c + 1) continue;
                int id2 = tR * _cols + tC - p.xoffset;
                // 如果放置不能匹配
                auto tmp = (p.bits << id2);
                if ((P & tmp).any()) continue;

                P |= tmp;
                //Ans.push_back(i);
                _dfs(S ^ (1 << i), tR * _cols + tC + 1, P, numSolutions);
                // Ans.pop_back();
                P ^= tmp;
            }
        }
    }
}
