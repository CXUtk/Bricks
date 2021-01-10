#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include "Render/Shader/ShaderData.h"

class TextRenderer {
public:
    TextRenderer(std::shared_ptr<ShaderData> shaderData);
    ~TextRenderer();

    // 注意这里的位置是文字的左上角
    void drawText(glm::vec2 pos, const std::string& text, float scale, const glm::vec3& color);
    glm::ivec2 measureString(const std::string& font, const std::string& text, float scale) const;

private:
    GLuint _vao, _vbo, _ebo;
    std::shared_ptr<ShaderData> _shaderData;

    void draw_quad(GLuint textureId, glm::vec2 bottomLeft, glm::vec2 size);
};
