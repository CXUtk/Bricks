#include "TriangleRenderer.h"
#include "Render/GLUtils.h"
#include "Game.h"

TriangleRenderer::TriangleRenderer(std::shared_ptr<ShaderData> shaderData) :_shaderData(shaderData) {
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);


    glBindVertexArray(_vao);

    // 绑定VAO到三个顶点
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 3, nullptr, GL_DYNAMIC_DRAW);


    // 替换顶点数据到当前Quad
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

TriangleRenderer::~TriangleRenderer() {
}

void TriangleRenderer::drawTriangle(glm::vec2 A, glm::vec2 B, glm::vec2 C, glm::vec2 pos, glm::vec2 origin, float scale, float rotation, const glm::vec3& color) {
    auto& game = Game::GetInstance();
    _shaderData->apply();
    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(pos, 0));

    model = glm::translate(model, glm::vec3(origin, 0));
    model = glm::rotate(model, rotation, glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(scale, scale, 1));
    model = glm::translate(model, glm::vec3(-origin, 0));
    glm::vec4 vertices[3] = {
        glm::vec4(A, 0, 0),
        glm::vec4(B, 0, 0),
        glm::vec4(C, 0, 0),
    };

    glUniformMatrix4fv(glGetUniformLocation(_shaderData->getID(), "model"), 1, false, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(_shaderData->getID(), "projection"), 1, false, glm::value_ptr(game.getGraphics()->getProjectionMatrix()));
    glUniform3f(glGetUniformLocation(_shaderData->getID(), "uColor"), color.r, color.g, color.b);


    glBindVertexArray(_vao);

    // 替换顶点数据到当前三角形
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 绘制三角形
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

}
