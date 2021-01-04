#include "Graphic.h"
#include "Game.h"
#include <glm/gtx/transform.hpp>

Graphics::Graphics() {
    auto shaderManager = Game::GetInstance().getShaderManager();
    _lineRenderer = std::unique_ptr<LineRenderer>(new LineRenderer(shaderManager->getShaderData("line_draw")));
    _quadRenderer = std::unique_ptr<QuadRenderer>(new QuadRenderer(shaderManager->getShaderData("quad_draw")));
    _spriteRenderer = std::unique_ptr<SpriteRenderer>(new SpriteRenderer(shaderManager->getShaderData("sprite_draw")));

    auto& game = Game::GetInstance();
    _projMatrix = glm::ortho(0.f, (float)game.getWidth(), 0.f, (float)game.getHeight(), -1.0f, 1.0f);
}

Graphics::~Graphics() {
}

void Graphics::drawLines(const std::vector<glm::vec2>& lines, const glm::vec3& color, float width) {
    _lineRenderer->drawLines(lines, color, width);
}

void Graphics::drawQuad(glm::vec2 pos, glm::vec2 size, const glm::vec3& color) {
    _quadRenderer->drawQuad(pos, size, color);
}

void Graphics::drawSprite(std::shared_ptr<Texture2D> texture, glm::vec2 pos, float scale, const glm::vec3& color) {
    glm::vec2 origin = glm::vec2(0.f, 0.f);
    _spriteRenderer->drawSprite(texture, pos, texture->getSize(), origin, scale, 0, color);
}

//void Graphics::drawSprite(std::shared_ptr<Texture2D> texture, glm::vec2 pos, glm::vec2 size, float scale, const glm::vec3& color) {
//    glm::vec2 origin = glm::vec2(0.f, 0.f);
//    _spriteRenderer->drawSprite(texture, pos, size, origin, scale, 0, color);
//}

void Graphics::drawSprite(std::shared_ptr<Texture2D> texture, glm::vec2 pos, glm::vec2 origin, float scale, float rotation, const glm::vec3& color) {
    _spriteRenderer->drawSprite(texture, pos, texture->getSize(), origin, scale, rotation, color);
}

glm::ivec2 Graphics::measureString(const std::string& font, const std::string& text, float scale) const {
    return glm::ivec2();
}
