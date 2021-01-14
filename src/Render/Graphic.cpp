#include "Graphic.h"
#include "Game.h"
#include <glm/gtx/transform.hpp>
#include <algorithm>

Graphics::Graphics() {
    auto shaderManager = Game::GetInstance().getShaderManager();
    _lineRenderer = std::unique_ptr<LineRenderer>(new LineRenderer(shaderManager->getShaderData("line_draw")));
    _quadRenderer = std::unique_ptr<QuadRenderer>(new QuadRenderer(shaderManager->getShaderData("quad_draw")));
    _circleRenderer = std::unique_ptr<QuadRenderer>(new QuadRenderer(shaderManager->getShaderData("circle_draw")));
    _spriteRenderer = std::unique_ptr<SpriteRenderer>(new SpriteRenderer(shaderManager->getShaderData("sprite_draw")));
    _textRenderer = std::unique_ptr<TextRenderer>(new TextRenderer(shaderManager->getShaderData("font_draw")));
    _triangleRenderer = std::unique_ptr<TriangleRenderer>(new TriangleRenderer(shaderManager->getShaderData("quad_draw")));
    auto& game = Game::GetInstance();
    _projMatrix = glm::scale(glm::vec3(1, -1, 1)) * glm::ortho(0.f, (float)game.getWidth(), 0.f, (float)game.getHeight(), -1.0f, 1.0f);
}

Graphics::~Graphics() {
}

void Graphics::drawLines(const std::vector<Segment>& lines, const glm::vec3& color, float width) {
    _lineRenderer->drawLines(lines, color, width);
}

void Graphics::drawDirectedArrow(glm::vec2 start, glm::vec2 end, const glm::vec3& color, float width, std::vector<Segment>& lines) {
    float triangleSize = std::max(20.f, width * 5);
    auto unit = glm::normalize(end - start);
    lines.push_back(Segment(start, end /*- unit * 0.5f * triangleSize*/));
    float rad = GLUtils::getRadians(unit);


    static glm::vec2 triangle[3] = {
        glm::vec2(0, 0),
        glm::vec2(-0.5, 0.2),
        glm::vec2(-0.5, -0.2),
    };
    //_triangleRenderer->drawTriangle(triangle[0], triangle[1], triangle[2], end, glm::vec2(0, 0), triangleSize, rad, color);
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

void Graphics::drawCircle(glm::vec2 pos, glm::vec2 size, const glm::vec3& color) {
    _circleRenderer->drawQuad(pos, size, color);
}

void Graphics::drawText(glm::vec2 pos, const std::string& text, float scale, const glm::vec3& color) {
    _textRenderer->drawText(pos, text, scale, color);
}

void Graphics::drawTriangles(const std::vector<Triangle>& triangles, const glm::mat4& transform, const glm::vec3& color) {
    _triangleRenderer->drawTriangles(triangles, transform, color);
}

glm::ivec2 Graphics::measureString(const std::string& font, const std::string& text, float scale) const {
    return _textRenderer->measureString(font, text, scale);
}
