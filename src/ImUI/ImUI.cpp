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

bool ImUI::button(std::shared_ptr<Texture2D> texture, glm::vec2 pos, float scale, const glm::vec3& color) {
    Game& game = Game::GetInstance();
    auto input = game.getInputManager();
    bool clicked = false;
    if (!input->getOldMouseDown() && input->getCurMouseDown() && inside(input->getMousePosition(), pos, glm::vec2(texture->getSize()) * scale)) {
        clicked = true;
    }
    game.getGraphics()->drawSprite(texture, pos, scale, color);
    return clicked;
}
