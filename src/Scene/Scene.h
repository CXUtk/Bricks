#pragma once

#include <memory>
#include <string>
#include <glm/glm.hpp>

class Scene {
public:
    static std::shared_ptr<Scene> makeScene(const std::string& name);

    Scene() = default;
    virtual void update() = 0;
    virtual void draw() = 0;
    virtual ~Scene() = 0 {};

private:
};
