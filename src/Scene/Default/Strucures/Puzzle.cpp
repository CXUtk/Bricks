#include "Puzzle.h"
#include <set>
#include <algorithm>





Puzzle::Puzzle() {
    _rows = 0, _cols = 0;
    _solveThread = nullptr;
    _solver = nullptr;
    _puzzleState = 0;
    _solving = false;
}

Puzzle::~Puzzle() {
    delete _solver;
}


void Puzzle::setFrameSize(int r, int c) {
    _rows = r, _cols = c;
}

void Puzzle::add(const Shape& shape) {
    _shapes.push_back(shape);
}

void Puzzle::build() {
    remove_dupStrongI();
}

void Puzzle::solve() {
    init_dlx();
    if (_solveThread) {
        _solver->found = true;
        if (_solveThread->joinable())
            _solveThread->join();
    }
    _solving = true;
    _solveThread = std::make_shared<std::thread>([&]() {
        _solver->solve();
        _solving = false;
        }
    );
    // _solveThread->join();
    //_solver->solve();
}

void Puzzle::stop() {
    if (_solveThread) {
        _solver->found = true;
        if (_solveThread->joinable())
            _solveThread->join();
    }
    _solving = false;
}

void Puzzle::place(int id, const Shape& shape, int r, int c) {
    assert((_puzzleState & (shape.gBit(_cols) << (r * _cols + c))) == 0);
    _puzzleState |= shape.gBit(_cols) << (r * _cols + c);
    _curShapeDups[id]--;
}

void Puzzle::unplace(int id, const std::bitset<MAX_SHAPE_SIZE>& S, int r, int c) {
    _puzzleState ^= S;
    assert((_puzzleState & S) == 0);
    _curShapeDups[id]++;
}

void Puzzle::clear() {
    _puzzleState = 0;
    _curShapeDups = _shapeDups;
}


std::vector<Shape_Info> Puzzle::getResultIM() {
    _results.clear();
    std::vector<int> tmp = _solver->getIntermidiateResult();
    for (int i = 0; i < tmp.size(); i++) {
        auto& info = _shapeInfo[tmp[i]];
        _results.push_back(info);
    }
    return _results;
}

Shape Puzzle::getShape(Shape_Info info, bool& extra) const {
    extra = info.id >= _shapes.size();
    if (extra) return Shape(1, 1, "O");
    return _shapes[info.id];
}


void Puzzle::remove_dupI() {
    std::sort(_shapes.begin(), _shapes.end());

    int sz = _shapes.size();
    int cnt = 0;
    std::vector<Shape> shapes;

    for (int i = 0; i < sz; i++) {
        cnt++;
        if (i == sz - 1 || !(_shapes[i] == _shapes[i + 1])) {
            shapes.push_back(_shapes[i]);
            _shapeDups[i] = cnt;
            cnt = 0;
        }
    }
    _shapes = shapes;
}

void Puzzle::remove_dupStrongI() {
    std::vector<Shape> shapes;
    int sz = _shapes.size();
    std::sort(_shapes.begin(), _shapes.end(), [](const Shape& a, const Shape& b) {
        return a.count() < b.count();
        });
    for (int i = 0; i < sz; i++) {
        Shape shape = _shapes[i];
        bool good = true;
        int badIndex = -1;
        for (int a = 0; a < 2; a++) {
            for (int b = 0; b < 4; b++) {
                for (int j = 0; j < shapes.size(); j++) {
                    if (shape == shapes[j]) {
                        good = false;
                        badIndex = j;
                        goto BAD_JUDGE;
                    }
                }
                shape = shape.rotateClockwise();
            }
            shape = shape.flip();
        }

    BAD_JUDGE:
        if (good) {
            shapes.push_back(_shapes[i]);
            _shapeDups.push_back(1);
        }
        else {
            _shapeDups[badIndex]++;
        }
    }
    _shapes = shapes;

    _curShapeDups = _shapeDups;
}

void Puzzle::init_dlx() {
    int shapeCount = _shapes.size();
    int numBlocks = 0;
    for (int i = 0; i < _cols * _rows; i++) {
        if (!_puzzleState[i]) {
            _idMap[i] = numBlocks;
            numBlocks++;
        }
    }

    _shapeInfo.clear();
    _shapeInfo.push_back({ -1,-1,-1,-1 });

    int activeBlocks = 0;
    for (int i = 0; i < shapeCount; i++) activeBlocks += _shapes[i].count() * _curShapeDups[i];

    if (numBlocks - activeBlocks > 0) {
        shapeCount++;
    }
    int dlxCols = shapeCount + numBlocks;

    if (_solver)
        delete _solver;
    _solver = new DLXSolver(5000, dlxCols);
    _solver->setConfigInfo(numBlocks - activeBlocks > 0 ? shapeCount - 1 : shapeCount, _rows, _cols);
    if (numBlocks - activeBlocks > 0) {

        for (int i = 0; i < _rows; i++) {
            for (int j = 0; j < _cols; j++) {
                int id = _shapeInfo.size();
                _shapeInfo.push_back(Shape_Info(shapeCount, i, j, 0));
                _solver->link(id, shapeCount);
                _solver->link(id, shapeCount + 1 + getID(i, j));
            }
        }
        _solver->setColDuplicates(shapeCount, numBlocks - activeBlocks);
    }


    for (int i = 0; i < _shapes.size(); i++) {
        Shape shape = _shapes[i];
        if (!_curShapeDups[i]) {
            _solver->remove(i + 1);
            continue;
        }
        _solver->setColDuplicates(i + 1, _curShapeDups[i]);

        // 判重+编号加入DLX
        std::set<Shape> dup;
        for (int a = 0; a < 2; a++) {
            for (int b = 0; b < 4; b++) {
                if (!dup.count(shape)) {
                    dup.insert(shape);
                    _solver->addBruteInfo(i, { shape.getTopLeftFittingXOffset(), shape.rows, shape.cols, shape.gBit(_cols) });

                    for (int r = 0; r < _rows - shape.rows + 1; r++) {
                        for (int c = 0; c < _cols - shape.cols + 1; c++) {


                            bool can = true;
                            for (int s = 0; s < shape.rows; s++) {
                                for (int t = 0; t < shape.cols; t++) {
                                    if (_puzzleState[(s + r) * _cols + t + c] && shape.bits.test(s * shape.cols + t)) {
                                        can = false;
                                        goto END_LOOP;
                                    }
                                }
                            }

                        END_LOOP:
                            if (!can) continue;

                            int id = _shapeInfo.size();
                            _shapeInfo.push_back(Shape_Info(i, r, c, a << 2 | b));
                            _solver->link(id, i + 1);

                            for (int s = 0; s < shape.rows; s++) {
                                for (int t = 0; t < shape.cols; t++) {
                                    if (shape.bits.test(s * shape.cols + t)) {
                                        int cID = getID(s + r, t + c);
                                        _solver->link(id, shapeCount + 1 + cID);

                                    }
                                }
                            }
                        }
                    }
                }
                shape = shape.rotateClockwise();
            }
            shape = shape.flip();
        }
    }
    printf("%d\n", _shapeInfo.size());
}
