#include "Font.h"
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Font::Font(std::map<char, Character> charInfo) :_charInfo(charInfo) {
}

Font::~Font() {
    std::cout << "Font has been released" << std::endl;
}
