#include "ImUI.h"
#include "Game.h"

static int activeItem;
static int globalID;


static Rect containerRect;
static Rect containerStack[255];
static int containerStackTop;

void pushCurrentContainer() {
    containerStack[++containerStackTop] = containerRect;
}

void popCurrentContainer() {
    containerRect = containerStack[containerStackTop--];
}

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
    Game& game = Game::GetInstance();
    globalID = 0;
    containerRect = Rect(glm::vec2(0, 0), glm::vec2(game.getWidth(), game.getHeight()));
}

void ImUI::EndGUI() {
    Game& game = Game::GetInstance();
    auto input = game.getInputManager();
    if (input->getCurMouseDown()) {
        // printf("%d\n", activeItem);
        if (activeItem == 0) {
            activeItem = -1;
        }
    }
    else {
        activeItem = 0;
    }
}

bool ImUI::img_button(std::shared_ptr<Texture2D> texture, glm::vec2 pos, glm::vec2 size, float scale, const glm::vec3& color, const glm::vec3& borderColor) {
    int id = ++globalID;
    Game& game = Game::GetInstance();
    auto input = game.getInputManager();
    bool clicked = false;
    pos += containerRect.pos;
    if (!input->getOldMouseDown() && input->getCurMouseDown() && activeItem == 0 && inside(input->getMousePosition(), pos, size)) {
        clicked = true;
        activeItem = id;
    }

    drawLineRect(pos, size, borderColor);
    game.getGraphics()->drawSprite(texture, pos + size * 0.5f, glm::vec2(0.5, 0.5), scale, 0.f, color);
    return clicked;
}

bool ImUI::pure_button(glm::vec2 pos, glm::vec2 size, const glm::vec3& color, const glm::vec3& borderColor, const std::string& text, const glm::vec3& textColor) {
    int id = ++globalID;
    Game& game = Game::GetInstance();
    auto input = game.getInputManager();
    bool clicked = false;
    pos += containerRect.pos;
    if (!input->getOldMouseDown() && input->getCurMouseDown() && activeItem == 0 && inside(input->getMousePosition(), pos, size)) {
        clicked = true;
        activeItem = id;
    }
    game.getGraphics()->drawQuad(pos, size, color);
    drawLineRect(pos, size, borderColor);

    glm::vec2 fsize = game.getGraphics()->measureString("default", text, 1.f);
    game.getGraphics()->drawText(pos + size * 0.5f - fsize * 0.5f, text, 1.f, textColor);
    return clicked;
}

bool ImUI::slider(glm::vec2 pos, int height, int max, int& value) {
    int id = ++globalID;
    Game& game = Game::GetInstance();
    auto input = game.getInputManager();
    pos += containerRect.pos;
    int ypos = ((height - 16 - 16) * value) / max;
    glm::vec3 buttonColor = glm::vec3(0.5f);
    if (inside(input->getMousePosition(), glm::vec2(pos.x + 8, pos.y + 8), glm::vec2(16, height - 16))) {
        if (!input->getOldMouseDown() && input->getCurMouseDown() && activeItem == 0) {
            activeItem = id;
        }
        buttonColor = glm::vec3(0.9f);
    }

    bool valueChanged = false;
    if (activeItem == id) {

        buttonColor = glm::vec3(0.9f);
        int mouseY = input->getMousePosition().y - (pos.y + 8);
        if (mouseY < 0) mouseY = 0;
        if (mouseY > height - 16) mouseY = height - 16;
        int v = max * mouseY / (height - 16);
        if (v != value) {
            value = v;
            valueChanged = true;
        }
    }

    game.getGraphics()->drawQuad(pos, glm::vec2(32, height), glm::vec3(0.2f));
    game.getGraphics()->drawQuad(glm::vec2(pos.x + 8, pos.y + 8 + ypos), glm::vec2(16, 16), buttonColor);
    //drawLineRect(glm::vec2(pos.x + 8, pos.y + 8), glm::vec2(16, height - 16), glm::vec3(1, 0, 0));
    return valueChanged;
}

void ImUI::BeginFrame(glm::vec2 pos, glm::vec2 size, const glm::vec3& color) {
    pushCurrentContainer();
    containerRect = Rect(containerRect.pos + pos, size);
    int id = ++globalID;
    drawLineRect(containerRect.pos, containerRect.size, color);
}

void ImUI::EndFrame() {
    popCurrentContainer();
}

Rect ImUI::getContainerRect() {
    return containerRect;
}
