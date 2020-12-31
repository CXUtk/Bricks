#include "ShaderManager.h"

static std::string defaultVert2Path = "Resources/Shaders/color.vs";
static std::string defaultVert4Path = "Resources/Shaders/default.vs";
static std::string defaultFragPath = "Resources/Shaders/color.frag";
static std::string spriteFragPath = "Resources/Shaders/sprite.frag";
static std::string spriteVertPath = "Resources/Shaders/sprite.vs";

ShaderManager::ShaderManager() {
    auto default2Shader = std::make_shared<ShaderData>(ShaderData::loadShaderProgram(defaultVert2Path, defaultFragPath));
    _shaders["line_draw"] = default2Shader;

    auto default4Shader = std::make_shared<ShaderData>(ShaderData::loadShaderProgram(defaultVert4Path, defaultFragPath));
    _shaders["quad_draw"] = default4Shader;

    auto defaultSpriteShader = std::make_shared<ShaderData>(ShaderData::loadShaderProgram(spriteVertPath, spriteFragPath));
    _shaders["sprite_draw"] = defaultSpriteShader;
}

ShaderManager::~ShaderManager() {
}

void ShaderManager::update(float delta) {
}
