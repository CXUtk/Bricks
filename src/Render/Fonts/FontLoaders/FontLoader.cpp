#include "FontLoader.h"
#include "FTLoader.h"
#include <iostream>

FontLoader::~FontLoader() {
    std::cout << "FontLoader has been released" << std::endl;
}

std::unique_ptr<FontLoader> FontLoader::makeLoader(FontLoaderType loader) {
    switch (loader) {
    case FontLoaderType::FreeType:
        return std::make_unique<FTLoader>();
    default:
        break;
    }
    throw std::exception("No such font loader type");
}
