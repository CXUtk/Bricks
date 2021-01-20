#pragma once

#include "../Scene.h"

class MinimumCircumscribedCircleScene : public Scene {
public:
    MinimumCircumscribedCircleScene();
    ~MinimumCircumscribedCircleScene() override;

    void update() override;
    void draw() override;

private:
    void insert(int x);
};
