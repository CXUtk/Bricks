#include "SpriteRenderer.h"
#include "Render/GLUtils.h"
#include "Game.h"


SpriteRenderer::SpriteRenderer(std::shared_ptr<ShaderData> shaderData) :_shaderData(shaderData) {
    GLUtils::genQuad(_vao, _vbo, _ebo);
}

SpriteRenderer::~SpriteRenderer() {
}

void SpriteRenderer::drawSprite(std::shared_ptr<Texture2D> texture, glm::vec2 pos, glm::vec2 size, glm::vec2 origin, float scale, float rotation, const glm::vec3& color) {
    auto& game = Game::GetInstance();
    _shaderData->apply();
    auto model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(pos, 0));

    model = glm::translate(model, glm::vec3(origin, 0));
    model = glm::rotate(model, rotation, glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(size.x * scale, size.y * scale, 1));
    model = glm::translate(model, glm::vec3(-origin, 0));

    glUniformMatrix4fv(glGetUniformLocation(_shaderData->getID(), "model"), 1, false, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(_shaderData->getID(), "projection"), 1, false, glm::value_ptr(game.getGraphics()->getProjectionMatrix()));
    glUniform3f(glGetUniformLocation(_shaderData->getID(), "uColor"), color.r, color.g, color.b);
    glUniform1i(glGetUniformLocation(_shaderData->getID(), "uTexture"), 0);

    glm::vec4 vertices[4] = {
        // 左下角
        glm::vec4(0, 0, 0, 0),
        // 右上角
        glm::vec4(1, 1, 1, 1),
        // 左上角
        glm::vec4(0, 1, 0, 1),
        // 右下角
        glm::vec4(1, 0, 1, 0),
    };


    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(_vao);

    glBindTexture(GL_TEXTURE_2D, texture->getId());
    // 替换顶点数据到当前Quad
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 绘制Quad
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
