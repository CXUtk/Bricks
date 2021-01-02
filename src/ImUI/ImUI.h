#pragma once
#include <vector>
#include <bitset>
#include <glm/glm.hpp>
#include <memory>
#include "Render/Texture2D.h"

class ImUI {
public:
    static ImUI& GetInstance();
    ~ImUI();
    bool button(std::shared_ptr<Texture2D> texture, glm::vec2 pos, float scale, const glm::vec3& color);

private:
    ImUI() = default;
};
