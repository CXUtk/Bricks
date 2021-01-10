#include "LineRenderer.h"
#include <algorithm>
#include "Game.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

LineRenderer::LineRenderer(std::shared_ptr<ShaderData> shaderData) :_shaderData(shaderData) {
    // 画线的顶点缓存
    glGenVertexArrays(1, &_vaoLine);
    glGenBuffers(1, &_vboLine);

    glBindVertexArray(_vaoLine);
    glBindBuffer(GL_ARRAY_BUFFER, _vboLine);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Segment) * BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

LineRenderer::~LineRenderer() {
}

void LineRenderer::drawLines(const std::vector<Segment>& segments, const glm::vec3& color, float width) {
    glLineWidth(width);

    auto& game = Game::GetInstance();
    auto model = glm::identity<glm::mat4>();
    _shaderData->apply();
    glUniformMatrix4fv(glGetUniformLocation(_shaderData->getID(), "projection"), 1, false, glm::value_ptr(game.getGraphics()->getProjectionMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(_shaderData->getID(), "model"), 1, false, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(_shaderData->getID(), "uColor"), color.r, color.g, color.b);


    int sz = segments.size() * 2;
    glBindVertexArray(_vaoLine);
    // 以 BUFFER_SIZE 个点为单位，分批绘制线段
    for (int i = 0; i < sz; i += BUFFER_SIZE) {
        glBindBuffer(GL_ARRAY_BUFFER, _vboLine);
        int count = std::min(sz - i, i + BUFFER_SIZE) - i;
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Segment) * count, segments.data() + i);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_LINES, 0, count);
    }

    glBindVertexArray(0);
}
