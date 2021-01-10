#include "ImUI.h"
#include "Game.h"

static int activeItem;
static int globalID;


bool inside(glm::vec2 mousePos, glm::vec2 pos, glm::vec2 size) {
    return mousePos.x >= pos.x && mousePos.x <= pos.x + size.x
        && mousePos.y >= pos.y && mousePos.y <= pos.y + size.y;
}


void drawLineRect(glm::vec2 pos, glm::vec2 size, const glm::vec3& color) {
    Game& game = Game::GetInstance();
    std::vector<Segment> lines;
    lines.push_back(Segment(pos, glm::vec2(pos.x + size.x, pos.y)));
    lines.push_back(Segment(pos, glm::vec2(pos.x, pos.y + size.y)));
    lines.push_back(Segment(glm::vec2(pos.x + size.x, pos.y), glm::vec2(pos.x + size.x, pos.y + size.y)));
    lines.push_back(Segment(glm::vec2(pos.x, pos.y + size.y), glm::vec2(pos.x + size.x, pos.y + size.y)));

    game.getGraphics()->drawLines(lines, color, 1);
}


void ImUI::BeginGUI() {
    activeItem = 0;
    globalID = 0;
}

bool ImUI::img_button(std::shared_ptr<Texture2D> texture, glm::vec2 pos, glm::vec2 size, float scale, const glm::vec3& color, const glm::vec3& borderColor) {
    Game& game = Game::GetInstance();
    auto input = game.getInputManager();
    bool clicked = false;
    if (!input->getOldMouseDown() && input->getCurMouseDown() && inside(input->getMousePosition(), pos, size)) {
        clicked = true;
    }

    drawLineRect(pos, size, borderColor);
    game.getGraphics()->drawSprite(texture, pos + size * 0.5f, glm::vec2(0.5, 0.5), scale, 0.f, color);
    return clicked;
}

bool ImUI::pure_button(glm::vec2 pos, glm::vec2 size, const glm::vec3& color, const glm::vec3& borderColor, const std::string& text, const glm::vec3& textColor) {
    Game& game = Game::GetInstance();
    auto input = game.getInputManager();
    bool clicked = false;
    if (!input->getOldMouseDown() && input->getCurMouseDown() && inside(input->getMousePosition(), pos, size)) {
        clicked = true;
    }

    game.getGraphics()->drawQuad(pos, size, color);
    drawLineRect(pos, size, borderColor);

    glm::vec2 fsize = game.getGraphics()->measureString("default", text, 1.f);
    game.getGraphics()->drawText(pos + size * 0.5f - fsize * 0.5f, text, 1.f, textColor);
    return clicked;
}
