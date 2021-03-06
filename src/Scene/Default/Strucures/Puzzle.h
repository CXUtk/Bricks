﻿#pragma once

#include <glm/glm.hpp>
#include <bitset>
#include <memory>
#include <vector>
#include "Algorithm/DLXSolver.h"
#include "Shape.h"


struct Shape_Info {
    int id;
    int r, c, s;
    Shape_Info() = default;
    Shape_Info(int id, int r, int c, int s) : id(id), r(r), c(c), s(s) {}
};

class Puzzle {
public:
    Puzzle();
    ~Puzzle();

    void setFrameSize(int r, int c);
    void add(const Shape& shape, int cnt);
    void build();


    void solve();
    void stop();

    void place(int id, const Shape& shape, int r, int c);
    void unplace(int id, const std::bitset<MAX_SHAPE_SIZE>& S);
    void place2(int id, const Shape& shape, int r, int c);
    void undo();
    void clear();
    int getCurCount(int id) const {
        return _curShapeDups[id];
    }

    bool isRunning() const {
        return _solving;
    }

    // 获取DLX当前求解的局面，确保线程安全
    std::vector<Shape_Info> getResultIM();
    std::vector<Shape> getShapeList() const { return _shapes; }

    Shape getShape(Shape_Info info, bool& extra) const;

    std::bitset<MAX_SHAPE_SIZE> getMask() const {
        return _puzzleState;
    }
private:

    int _rows, _cols;
    bool _solving;

    std::vector<Shape> _shapes;
    std::vector<int> _shapeDups;
    std::vector<int> _curShapeDups;


    std::vector<Shape_Info> _shapeInfo;
    // Shape _mainFrame;
    DLXSolver* _solver;
    BruteForce* _solver2;



    std::shared_ptr<std::thread> _solveThread;

    std::bitset<MAX_SHAPE_SIZE> _puzzleState;


    struct PlaceAction {
        int id;
        std::bitset<MAX_SHAPE_SIZE> S;
    };

    std::vector<PlaceAction> _placeActionStack;
    int _idMap[MAX_SHAPE_SIZE];


    // 判重，移除相同块，有下面那个还要这个干嘛？？
    void remove_dupI();
    // 判重，移除本质相同块
    void remove_dupStrongI();


    void init_dlx();
    int getID(int r, int c) const { 
        return _idMap[r * _cols + c]; }

};
