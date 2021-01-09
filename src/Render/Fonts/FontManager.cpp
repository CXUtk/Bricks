#include <iostream>

#include "FontManager.h"
#include "FontLoaders/FontLoader.h"

FontManager::FontManager() {
    auto loader = FontLoader::makeLoader(FontLoaderType::FreeType);
    _fontMap["default"] = loader->loadFont("Resources/Fonts/arial.ttf", 32);
}

FontManager::~FontManager() {
    std::cout << "FontManager has been released" << std::endl;
}
