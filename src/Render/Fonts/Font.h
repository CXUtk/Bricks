#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <map>
#include <glm/glm.hpp>


/*
    Character 类型存储单个字符的绘制信息
*/
struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

/*
    Font 类型存储有关于这个字体的绘制信息
*/
class Font {
public:
    Font(std::map<char, Character> charInfo);
    ~Font();
    const Character& operator[](const char c) { return _charInfo[c]; }

private:
    std::map<char, Character> _charInfo;
};
