﻿#pragma once
#include <vector>
#include <algorithm>
#include <random>
#include <glm/glm.hpp>

constexpr int MAXN = 100005;
constexpr int MAX_VERTICES = 30;
constexpr double EPS = 1e-9;

static std::mt19937 mt;

int dcmp(double x);

struct Vector2 {
    double x, y;
    Vector2() :x(0), y(0) {}
    Vector2(double x, double y) : x(x), y(y) {}
    Vector2(double rad) : x(cos(rad)), y(sin(rad)) {}

    glm::vec2 to_vec2() const {
        return glm::vec2(x, y);
    }
    glm::dvec2 to_dvec2() const {
        return glm::dvec2(x, y);
    }

    static double dot(const Vector2& a, const Vector2& b) {
        return a.x * b.x + a.y * b.y;
    }
    static double angle(const Vector2& a, const Vector2& b) {
        return acos(dot(a, b) / a.length() / b.length());
    }
    static double cross(const Vector2& a, const Vector2& b) {
        return a.x * b.y - a.y * b.x;
    }
    static double area2(const Vector2& a, const Vector2& b, const Vector2& c) {
        return cross(b - a, c - a);
    }

    double length() const { return sqrt(x * x + y * y); }
    double lengthSq() const { return x * x + y * y; }
    double getRad() const { return atan2(y, x); }
    Vector2 rotate(double rad) const {
        return Vector2(x * cos(rad) - y * sin(rad),
            x * sin(rad) + y * cos(rad));
    }
    Vector2 unit() const {
        double l = this->length();
        return Vector2(x / l, y / l);
    }
    Vector2 normal() const {
        double l = this->length();
        return Vector2(-y / l, x / l);
    }

    bool operator<(const Vector2& other) const {
        return x < other.x || (x == other.x && y < other.y);
    }
    bool operator==(const Vector2& other) const {
        return !dcmp(x - other.x) && !dcmp(y - other.y);
    }

    Vector2& operator+=(const Vector2& vec) {
        this->x += vec.x, this->y += vec.y;
        return *this;
    }
    Vector2 operator+(const Vector2& vec) const {
        return Vector2(this->x + vec.x, this->y + vec.y);
    }
    Vector2& operator-=(const Vector2& vec) {
        this->x -= vec.x, this->y -= vec.y;
        return *this;
    }
    Vector2 operator-(const Vector2& vec) const {
        return Vector2(this->x - vec.x, this->y - vec.y);
    }
    Vector2& operator*=(double v) {
        this->x *= v, this->y *= v;
        return *this;
    }
    Vector2& operator*=(const Vector2& v) {
        this->x *= v.x, this->y *= v.y;
        return *this;
    }
    Vector2 operator*(double v) const {
        return Vector2(this->x * v, this->y * v);
    }
    Vector2 operator*(const Vector2& vec) const {
        return Vector2(this->x * vec.x, this->y * vec.y);
    }
    Vector2& operator/=(double v) {
        this->x /= v, this->y /= v;
        return *this;
    }
    Vector2& operator/=(const Vector2& vec) {
        this->x /= vec.x, this->y /= vec.y;
        return *this;
    }
    Vector2 operator/(double v) const {
        return Vector2(this->x / v, this->y / v);
    }
    Vector2 operator/(const Vector2& vec) const {
        return Vector2(this->x / vec.x, this->y / vec.y);
    }
};


struct Face;
struct Vertex;
struct Edge;

struct Edge {
    Vertex* from;
    Vertex* to;
    Edge* twin;
    Face* face;

    Edge* prev;
    Edge* next;

    int id;

    Edge() : id(0), from(nullptr), to(nullptr), twin(nullptr), face(nullptr), prev(nullptr), next(nullptr) {}
    Edge(Vertex* fr, Vertex* to) : id(0), from(fr), to(to), twin(nullptr), face(nullptr), prev(nullptr), next(nullptr) {}
};

struct Line {
    Vector2 p, dir;
    Line() {}
    Line(const Vector2& p, const Vector2& dir) : p(p), dir(dir) {}
    static Vector2 lineIntersection(const Line& l1, const Line& l2) {
        double t = Vector2::cross(l2.dir, l1.p - l2.p) /
            Vector2::cross(l1.dir, l2.dir);
        return l1.p + l1.dir * t;
    }
    static double distanceToLine(const Vector2& p, const Line& line) {
        return fabs(Vector2::cross(line.dir, p - line.p)) / line.dir.length();
    }

    static Vector2 lineProjection(const Vector2& p, const Line& l) {
        return l.p + l.dir * Vector2::dot(l.dir, p - l.p) /
            Vector2::dot(l.dir, l.dir);
    }
};


struct Face {
    int id;
    Edge* edge;
    Vector2 vs[3];
    std::vector<Vertex*> owned;


    Face() : id(0), edge(nullptr) {
    }
    Face(int id, Edge* edge) : id(id), edge(edge) {
    }

    Vector2 getCircumcentre() const;
};

struct Vertex {
    int id;
    Face* belong;
    Vector2 pos;

    Vertex() :id(0), belong(nullptr), pos() {}
    Vertex(int id, double x, double y) : id(id), belong(nullptr), pos(x, y) {}

    void shake() {
        pos.x += (mt.max() / 2.0 - mt()) / (double)mt.max() * 2 * EPS;
        pos.y += (mt.max() / 2.0 - mt()) / (double)mt.max() * 2 * EPS;
    }

    bool testInTriangle(Face* face) {
        auto e = face->edge;
        for (int i = 0; i < 3; i++) {
            auto dir = e->to->pos - e->from->pos;
            auto p = pos - e->from->pos;
            if (Vector2::cross(dir, p) < 0) return false;
            e = e->next;
        }
        belong = face;
        face->owned.push_back(this);
        return true;
    }
};
