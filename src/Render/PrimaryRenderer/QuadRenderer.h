#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Render/Shader/ShaderData.h"

// 方块渲染器
class QuadRenderer {
public:
    QuadRenderer(std::shared_ptr<ShaderData> shaderData);
    ~QuadRenderer();

    void drawQuad(glm::vec2 pos, glm::vec2 size, const glm::vec3& color);
    void drawCircle(glm::vec2 pos, glm::vec2 size, const glm::vec3& color, float thickness);

private:

    GLuint _vaoQuad;
    GLuint _vboQuad;
    GLuint _ebo;

    std::shared_ptr<ShaderData> _shaderData;
};
