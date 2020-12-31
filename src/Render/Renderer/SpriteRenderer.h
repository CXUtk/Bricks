#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <glm/glm.hpp>
#include "Render/Texture2D.h"
#include "Render/Shader/ShaderData.h"

class SpriteRenderer {
public:
    SpriteRenderer(std::shared_ptr<ShaderData> shaderData);
    ~SpriteRenderer();

    void drawSprite(std::shared_ptr<Texture2D> texture, glm::vec2 pos, glm::vec2 size, glm::vec2 origin, float scale, float rotation, const glm::vec3& color);

private:
    GLuint _vao, _vbo, _ebo;
    std::shared_ptr<ShaderData> _shaderData;
};
