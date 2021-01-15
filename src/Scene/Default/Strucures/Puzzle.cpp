#include "Puzzle.h"
#include <set>
#include <algorithm>

Shape Shape::rotateClockwise() const {
    Shape out(cols, rows);
    out.bits = 0;
    for (int i = rows - 1; i >= 0; i--) {
        for (int j = 0; j < cols; j++) {
            int id = i * cols + j;
            if (bits.test(id)) {
                int id2 = j * rows + (rows - i - 1);
                out.bits.set(id2);
            }
        }
    }
    return out;
}

Shape Shape::flip() const {
    Shape out(rows, cols);
    out.bits = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int id = i * cols + j;
            if (bits.test(id)) {
                int id2 = i * cols + (cols - j - 1);
                out.bits.set(id2);
            }
        }
    }
    return out;
}

int Shape::count() const {
    int cnt = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int id = i * cols + j;
            if (bits.test(id)) {
                cnt++;
            }
        }
    }
    return cnt;
}







Puzzel::Puzzel() {
    _rows = 0, _cols = 0;
}

Puzzel::~Puzzel() {
    delete _solver;
}


void Puzzel::setFrameSize(int r, int c) {
    _rows = r, _cols = c;
}

void Puzzel::add(const Shape& shape) {
    _shapes.push_back(shape);
}

void Puzzel::build() {
    remove_dupStrongI();
    init_dlx();
}

void Puzzel::solve() {
}

void Puzzel::remove_dupI() {
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

void Puzzel::remove_dupStrongI() {
    std::vector<Shape> shapes;
    int sz = _shapes.size();

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
                shape.rotateClockwise();
            }
            shape.flip();
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
}

void Puzzel::init_dlx() {
    int shapeCount = _shapes.size();
    int numBlocks = _rows * _cols;


    _shapeInfo.push_back({ -1,-1,-1,-1 });

    int activeBlocks = 0;
    for (int i = 0; i < shapeCount; i++) activeBlocks += _shapes[i].count() * _shapeDups[i];

    if (activeBlocks > 0) {
        shapeCount++;
    }
    int dlxCols = shapeCount + numBlocks;

    _solver = new DLXSolver(5000, dlxCols);

    for (int i = 0; i < shapeCount; i++) {
        Shape shape = _shapes[i];
        _solver->setColDuplicates(i + 1, _shapeDups[i]);


        std::set<Shape> dup;
        for (int a = 0; a < 2; a++) {
            for (int b = 0; b < 4; b++) {
                if (!dup.count(shape)) {
                    dup.insert(shape);

                    for (int r = 0; r <= _rows - shape.rows + 1; r++) {
                        for (int c = 0; c <= _cols - shape.cols + 1; c++) {
                            int id = _shapeInfo.size();
                            _shapeInfo.push_back(Shape_Info(i, r, c, a << 2 | b));
                            _solver->link(id, i + 1);

                            for (int s = 0; s < shape.rows; s++) {
                                for (int t = 0; t < shape.cols; t++) {
                                    if (shape.bits.test(s * shape.cols + t)) {
                                        _solver->link(id, shapeCount + 1 + getID(s + r, t + c));
                                    }
                                }
                            }

                        }
                    }
                }
                shape.rotateClockwise();
            }
            shape.flip();
        }
    }

    if (activeBlocks > 0) {
        for (int i = 0; i < _rows; i++) {
            for (int j = 0; j < _cols; j++) {
                int id = _shapeInfo.size();
                _shapeInfo.push_back(Shape_Info(shapeCount, i, j, 0));
                _solver->link(id, shapeCount + 1 + getID(i, j));
            }
        }
    }

    printf("%d\n", _shapeInfo.size());
}
