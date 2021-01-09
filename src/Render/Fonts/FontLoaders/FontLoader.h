#pragma once

#include <memory>
#include "../Font.h"

enum class FontLoaderType {
    FreeType,
};

class FontLoader {
public:
    FontLoader() = default;
    virtual ~FontLoader() = 0;
    virtual std::shared_ptr<Font> loadFont(const std::string& path, int size) = 0;

    static std::unique_ptr<FontLoader> makeLoader(FontLoaderType loader);
};
