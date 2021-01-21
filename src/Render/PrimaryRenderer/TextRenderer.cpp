#include "TextRenderer.h"
#include <iostream>
#include <algorithm>
#include "Game.h"
#include "../GLUtils.h"

TextRenderer::TextRenderer(std::shared_ptr<ShaderData> shaderData) :_shaderData(shaderData) {
    GLUtils::genQuad(_vao, _vbo, _ebo);
}

TextRenderer::~TextRenderer() {
}

void TextRenderer::drawText(const std::string& font, glm::vec2 pos, const std::string& text, float scale, const glm::vec3& color) {
    auto& game = Game::GetInstance();
    _shaderData->apply();
    auto model = glm::identity<glm::mat4>();
    glUniformMatrix4fv(glGetUniformLocation(_shaderData->getID(), "model"), 1, false, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(_shaderData->getID(), "projection"), 1, false, glm::value_ptr(game.getGraphics()->getProjectionMatrix()));
    glUniform3f(glGetUniformLocation(_shaderData->getID(), "uColor"), color.r, color.g, color.b);
    glUniform1i(glGetUniformLocation(_shaderData->getID(), "uTexture"), 0);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(_vao);
    auto fontManager = Game::GetInstance().getFontManager();
    auto defaultFont = fontManager->getFont(font);

    auto size = measureString(font, text, scale);

    for (const auto c : text) {
        Character chr = (*defaultFont)[c];
        glm::vec2 topLeft = glm::vec2(pos.x + chr.Bearing.x * scale,
            (pos.y) - (chr.Bearing.y) * scale);
        glm::vec2 size = glm::vec2(chr.Size) * scale;
        draw_quad(chr.TextureID, topLeft, size);
        pos.x += chr.Advance / 64 * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
}

glm::ivec2 TextRenderer::measureString(const std::string& font, const std::string& text, float scale) const {
    glm::ivec2 result = glm::ivec2(0);
    auto fontManager = Game::GetInstance().getFontManager();
    std::shared_ptr<Font> defaultFont = fontManager->getFont(font);
    for (const auto c : text) {
        Character chr = (*defaultFont)[c];
        result.y = std::max(result.y, chr.Size.y);
        result.x += chr.Advance / 64;
    }
    return glm::ivec2((int)(result.x * scale), (int)(result.y * scale));
}

void TextRenderer::draw_quad(GLuint textureId, glm::vec2 bottomLeft, glm::vec2 size) {
    glm::vec4 vertices[4] = {
        // 左下角
        glm::vec4((int)bottomLeft.x, (int)bottomLeft.y, 0.f, 0.f),
        // 右上角
        glm::vec4((int)(bottomLeft.x + size.x), (int)(bottomLeft.y + size.y), 1.f, 1.f),
        // 左上角
        glm::vec4((int)bottomLeft.x, (int)(bottomLeft.y + size.y), 0.f, 1.f),
        // 右下角
        glm::vec4((int)(bottomLeft.x + size.x), (int)bottomLeft.y, 1.f, 0.f),
    };
    // 使用贴图
    glBindTexture(GL_TEXTURE_2D, textureId);

    // 替换顶点数据到当前Quad
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // 绘制Quad
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
