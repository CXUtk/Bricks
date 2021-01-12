#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Render/Shader/ShaderData.h"

class TriangleRenderer {
public:
    TriangleRenderer(std::shared_ptr<ShaderData> shaderData);
    ~TriangleRenderer();

    void drawTriangle(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 pos, glm::vec2 origin, float scale, float rotation, const glm::vec3& color);

private:

    GLuint _vao;
    GLuint _vbo;
    GLuint _ebo;

    std::shared_ptr<ShaderData> _shaderData;
};
