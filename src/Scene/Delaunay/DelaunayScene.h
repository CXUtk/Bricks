#pragma once
#include "../Scene.h"
class DelaunayScene : public Scene {
public:
    DelaunayScene();
    ~DelaunayScene() override;

    void update() override;
    void draw() override;

private:
    void insert(int x);
};
