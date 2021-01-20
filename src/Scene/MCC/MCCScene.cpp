#include "MCCScene.h"
#include "Game.h"

#include <glm/glm.hpp>
#include <vector>
#include <ctime>
#include <map>
#include <random>
#include <mutex>
#include <thread>

std::vector<glm::vec2> P;
static std::mt19937 mt;
static int timer;
static int curIndex;
static std::mutex _mutexLock;
static Segment indicator;
static Segment indicator2;
static std::thread* _mccThread;


struct Circle {
    glm::vec2 center;
    float r;
    Circle() = default;
    Circle(glm::vec2 center, float r) : center(center), r(r) {}

    bool test(const glm::vec2& p) const { return glm::distance(p, center) <= r; }
};


static Circle circle;

Circle getCircumscribedCircle(glm::vec2 A, glm::vec2 B, glm::vec2 C) {
    auto p1 = (A + B) * 0.5f;
    auto d1 = (B - A);
    d1 = glm::vec2(-d1.y, d1.x);

    auto p2 = (A + C) * 0.5f;
    auto d2 = (C - A);
    d2 = glm::vec2(-d2.y, d2.x);

    glm::mat2 M(d1, -d2);
    auto D = p2 - p1;
    D = glm::inverse(M) * D;
    auto center = p1 + D.x * d1;
    return Circle(center, glm::distance(A, center));
}


void drawCircle(glm::vec2 p, float r, int thickness) {
    auto& game = Game::GetInstance();
    game.getGraphics()->drawCircle(glm::vec2(p.x - r, p.y - r), glm::vec2(r, r) * 2.f, glm::vec3(1, 0, 0), 2 / (2.0f * r));
}

MinimumCircumscribedCircleScene::MinimumCircumscribedCircleScene() {
    mt.seed(12432);
    for (int i = 0; i < 10; i++) {
        P.push_back(glm::vec2(100 + mt() % 400, 100 + mt() % 400));
    }

    timer = 0;
    indicator = Segment(P[0], P[0]);
    indicator2 = indicator;
    curIndex = 0;
    circle = Circle((P[0] + P[1]) * 0.5f, glm::length(P[0] - P[1]) * 0.5f);

    _mccThread = new std::thread([&]() {
        int sz = P.size();
        for (int i = 2; i < sz; i++) {

            _mutexLock.lock();
            curIndex = i;
            _mutexLock.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (!circle.test(P[i])) {

                _mutexLock.lock();
                circle = Circle(P[i], 0);
                _mutexLock.unlock();

                for (int j = 0; j < i; j++) {
                    if (!circle.test(P[j])) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(200));
                        _mutexLock.lock();
                        indicator = Segment(P[i], P[j]);
                        circle = Circle((P[i] + P[j]) * 0.5f, glm::length(P[i] - P[j]) * 0.5f);
                        _mutexLock.unlock();
                        for (int k = 0; k < j; k++) {

                            std::this_thread::sleep_for(std::chrono::milliseconds(200));
                            if (!circle.test(P[k])) {
                                _mutexLock.lock();
                                indicator2 = Segment(P[j], P[k]);
                                circle = getCircumscribedCircle(P[i], P[j], P[k]);
                                _mutexLock.unlock();
                            }
                        }
                    }
                }
            }
        }

        });

}

MinimumCircumscribedCircleScene::~MinimumCircumscribedCircleScene() {
    delete _mccThread;
}


void MinimumCircumscribedCircleScene::update() {
}

void MinimumCircumscribedCircleScene::draw() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    _mutexLock.lock();
    auto& game = Game::GetInstance();
    for (int i = 0; i <= curIndex; i++) {
        auto& p = P[i];
        game.getGraphics()->drawCircle(glm::vec2(p.x - 3, p.y - 3), glm::vec2(6), glm::vec3(1, 1, 0), 0.5);
    }
    drawCircle(circle.center, circle.r, 1);

    std::vector<Segment> segs;
    segs.push_back(indicator);
    segs.push_back(indicator2);
    game.getGraphics()->drawLines(segs, glm::vec3(1), 1);

    _mutexLock.unlock();
    glDisable(GL_BLEND);
}
