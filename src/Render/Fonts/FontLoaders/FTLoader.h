#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <memory>

#include "../Font.h"
#include "FontLoader.h"

class FTLoader : public FontLoader {
public:
    FTLoader();
    ~FTLoader() override;

    std::shared_ptr<Font> loadFont(const std::string& path, int size);

private:
    FT_Library _ftLib;
};
