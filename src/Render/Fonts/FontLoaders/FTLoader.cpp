#include "FTLoader.h"
#include <GLFW/glfw3.h>
#include <iostream>

FTLoader::FTLoader() {
    if (FT_Init_FreeType(&_ftLib)) {
        throw std::exception("Cannot initialize FreeType Library");
    }
}

FTLoader::~FTLoader() {
    FT_Done_FreeType(_ftLib);
}


/*
    把字体位图变成OpenGL贴图
*/
GLuint genTexture(FT_Face face) {
    GLuint id;
    glGenTextures(1, &id);
    // 告诉GL这是2D贴图
    glBindTexture(GL_TEXTURE_2D, id);
    // 传输字体的位图数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
        face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return id;
}

std::shared_ptr<Font> FTLoader::loadFont(const std::string& path, int size) {

    FT_Face ftFace;
    if (FT_New_Face(_ftLib, path.c_str(), 0, &ftFace)) {
        throw std::exception("Cannot load font face");
    }
    FT_Set_Pixel_Sizes(ftFace, 0, size);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

    std::map<char, Character> data;
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(ftFace, c, FT_LOAD_RENDER)) {
            std::cerr << "Unable to load glyph character" << c << std::endl;
            continue;
        }
        GLuint tex = genTexture(ftFace);
        auto& bitmap = ftFace->glyph->bitmap;

        Character chr;
        chr.TextureID = tex;
        chr.Size = glm::ivec2(bitmap.width, bitmap.rows);
        chr.Bearing = glm::ivec2(ftFace->glyph->bitmap_left, ftFace->glyph->bitmap_top);
        chr.Advance = ftFace->glyph->advance.x;

        data.insert({ c, chr });
    }

    FT_Done_Face(ftFace);
    return std::make_shared<Font>(data);
}
