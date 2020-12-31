#include "Graphic.h"
#include "Game.h"
Graphics::Graphics() {
    auto shaderManager = Game::GetInstance().getShaderManager();
    _lineRenderer = std::unique_ptr<LineRenderer>(new LineRenderer(shaderManager->getShaderData("line_draw")));
    _quadRenderer = std::unique_ptr<QuadRenderer>(new QuadRenderer(shaderManager->getShaderData("quad_draw")));
}

Graphics::~Graphics() {
}

void Graphics::drawLines(const std::vector<glm::vec2>& lines, const glm::vec3& color, float width) {
    _lineRenderer->drawLines(lines, color, width);
}

void Graphics::drawQuad(glm::vec2 pos, glm::vec2 size, const glm::vec3& color) {
    _quadRenderer->drawQuad(pos, size, color);
}

glm::ivec2 Graphics::measureString(const std::string& font, const std::string& text, float scale) const {
    return glm::ivec2();
}
