﻿#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <memory>

#include "Renderer/LineRenderer.h"
#include "Renderer/QuadRenderer.h"
#include "Renderer/SpriteRenderer.h"
#include <string>
class Graphics {
public:
    Graphics();
    ~Graphics();

    void drawLines(const std::vector<glm::vec2>& lines, const glm::vec3& color, float width);
    void drawQuad(glm::vec2 pos, glm::vec2 size, const glm::vec3& color);
    void drawSprite(std::shared_ptr<Texture2D> texture, glm::vec2 pos, float scale, const glm::vec3& color);
    void drawSprite(std::shared_ptr<Texture2D> texture, glm::vec2 pos, glm::vec2 size, float scale, const glm::vec3& color);
    void drawSprite(std::shared_ptr<Texture2D> texture, glm::vec2 pos, glm::vec2 origin, float scale, float rotation, const glm::vec3& color);

    glm::ivec2 measureString(const std::string& font, const std::string& text, float scale) const;

    void setProjectionMatrix(const glm::mat4& mat) {
        _projMatrix = mat;
    }
    glm::mat4 getProjectionMatrix()const { return _projMatrix; }
private:
    std::unique_ptr<LineRenderer> _lineRenderer;
    std::unique_ptr<QuadRenderer> _quadRenderer;
    std::unique_ptr<SpriteRenderer> _spriteRenderer;

    glm::mat4 _projMatrix;
};