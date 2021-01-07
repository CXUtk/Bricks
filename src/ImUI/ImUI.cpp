#include "ImUI.h"
#include "Game.h"

ImUI& ImUI::GetInstance() {
    static ImUI ui;
    return ui;
}

ImUI::~ImUI() {
}

bool inside(glm::vec2 mousePos, glm::vec2 pos, glm::vec2 size) {
    return mousePos.x >= pos.x && mousePos.x <= pos.x + size.x
        && mousePos.y >= pos.y && mousePos.y <= pos.y + size.y;
}

bool ImUI::img_button(std::shared_ptr<Texture2D> texture, glm::vec2 pos, glm::vec2 size, float scale, const glm::vec3& color, const glm::vec3& borderColor) {
    Game& game = Game::GetInstance();
    auto input = game.getInputManager();
    bool clicked = false;
    if (!input->getOldMouseDown() && input->getCurMouseDown() && inside(input->getMousePosition(), pos, size)) {
        clicked = true;
    }
    std::vector<glm::vec2> lines;
    lines.push_back(pos), lines.push_back(glm::vec2(pos.x + size.x, pos.y));
    lines.push_back(pos), lines.push_back(glm::vec2(pos.x, pos.y + size.y));
    lines.push_back(glm::vec2(pos.x + size.x, pos.y)), lines.push_back(glm::vec2(pos.x + size.x, pos.y + size.y));
    lines.push_back(glm::vec2(pos.x, pos.y + size.y)), lines.push_back(glm::vec2(pos.x + size.x, pos.y + size.y));

    game.getGraphics()->drawLines(lines, borderColor, 1);
    game.getGraphics()->drawSprite(texture, pos + size * 0.5f, glm::vec2(0.5, 0.5), scale, 0.f, color);
    return clicked;
}

bool ImUI::pure_button(glm::vec2 pos, glm::vec2 size, const glm::vec3& color, const glm::vec3& borderColor) {
    Game& game = Game::GetInstance();
    auto input = game.getInputManager();
    bool clicked = false;
    if (!input->getOldMouseDown() && input->getCurMouseDown() && inside(input->getMousePosition(), pos, size)) {
        clicked = true;
    }
    std::vector<glm::vec2> lines;
    lines.push_back(pos), lines.push_back(glm::vec2(pos.x + size.x, pos.y));
    lines.push_back(pos), lines.push_back(glm::vec2(pos.x, pos.y + size.y));
    lines.push_back(glm::vec2(pos.x + size.x, pos.y)), lines.push_back(glm::vec2(pos.x + size.x, pos.y + size.y));
    lines.push_back(glm::vec2(pos.x, pos.y + size.y)), lines.push_back(glm::vec2(pos.x + size.x, pos.y + size.y));

    game.getGraphics()->drawQuad(pos, size, color);
    game.getGraphics()->drawLines(lines, borderColor, 1);
    return clicked;
}
