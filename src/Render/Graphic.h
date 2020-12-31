#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "Renderer/LineRenderer.h"
#include "Renderer/QuadRenderer.h"
#include <string>
class Graphics {
public:
    Graphics();
    ~Graphics();

    void drawLines(const std::vector<glm::vec2>& lines, const glm::vec3& color, float width);
    void drawQuad(glm::vec2 pos, glm::vec2 size, const glm::vec3& color);

    glm::ivec2 measureString(const std::string& font, const std::string& text, float scale) const;

private:
    std::unique_ptr<LineRenderer> _lineRenderer;
    std::unique_ptr<QuadRenderer> _quadRenderer;
};
