#include "QuadRenderer.h"
#include "Render/GLUtils.h"
#include "Game.h"

QuadRenderer::QuadRenderer(std::shared_ptr<ShaderData> shaderData) :_shaderData(shaderData) {
    GLUtils::genQuad(_vaoQuad, _vboQuad, _ebo);
    glm::vec4 vertices[4] = {
        // 左下角
        glm::vec4(0, 0, 0, 1),
        // 右上角
        glm::vec4(1, 1, 0, 1),
        // 左上角
        glm::vec4(0, 1, 0, 1),
        // 右下角
        glm::vec4(1, 0, 0, 1),
    };
    glBindVertexArray(_vaoQuad);
    // 替换顶点数据到当前Quad
    glBindBuffer(GL_ARRAY_BUFFER, _vboQuad);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

QuadRenderer::~QuadRenderer() {
}

void QuadRenderer::drawQuad(glm::vec2 pos, glm::vec2 size, const glm::vec3& color) {
    auto& game = Game::GetInstance();
    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(pos.x, pos.y, 0));
    model = glm::scale(model, glm::vec3(size.x, size.y, 1));
    _shaderData->apply();

    glUniformMatrix4fv(glGetUniformLocation(_shaderData->getID(), "model"), 1, false, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(_shaderData->getID(), "projection"), 1, false, glm::value_ptr(game.getGraphics()->getProjectionMatrix()));
    glUniform3f(glGetUniformLocation(_shaderData->getID(), "uColor"), color.r, color.g, color.b);


    glBindVertexArray(_vaoQuad);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
