#pragma once
#include <vector>
#include <bitset>
#include <glm/glm.hpp>
#include <memory>
#include "Render/Texture2D.h"


struct Rect {
    glm::vec2 pos, size;
    Rect() = default;
    Rect(glm::vec2 pos, glm::vec2 size) : pos(pos), size(size) {}

    glm::vec2 center() const {
        return pos + size * 0.5f;
    }
};

class ImUI {
public:
    static void BeginGUI();
    static void EndGUI();
    // 生成一个带有图片的按钮，左下角为pos，响应区域大小为size，同时图片会居中放置，以scale为大小整体缩放，绘制图片使用color染色
    static bool img_button(std::shared_ptr<Texture2D> texture, glm::vec2 pos, glm::vec2 size, float scale, const glm::vec3& color, const glm::vec3& borderColor);

    // 生成一个普通按钮，左下角为pos，响应区域大小为size
    static bool pure_button(glm::vec2 pos, glm::vec2 size, const glm::vec3& color, const glm::vec3& borderColor, const std::string& text, const glm::vec3& textColor);

    static bool slider(glm::vec2 pos, int height, int max, int& value);

    static void BeginScrollableArea(glm::vec2 pos, glm::vec2 size, int& scrollValue);
    static void EndScrollableArea();

    static void BeginFrame(glm::vec2 pos, glm::vec2 size, const glm::vec3& color);
    static void EndFrame();

    static Rect getContainerRect();

private:

};
