#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Render/Shader/ShaderData.h"
#include "Render/GLUtils.h"

// 线渲染器
class LineRenderer {
public:
    LineRenderer(std::shared_ptr<ShaderData> shaderData);
    ~LineRenderer();

    void drawLines(const std::vector<Segment>& segments, const glm::vec3& color, float width);

private:

    GLuint _vaoLine;
    GLuint _vboLine;

    std::shared_ptr<ShaderData> _shaderData;

    static constexpr int BUFFER_SIZE = 1024;
};
