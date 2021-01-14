#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Render/Shader/ShaderData.h"
#include <Render\GLUtils.h>

struct PolyominoRenderer {
public:
    PolyominoRenderer();
    ~PolyominoRenderer();

private:
    std::vector<Triangle> _backGround;
    std::vector<Triangle> _foreGround;
};
