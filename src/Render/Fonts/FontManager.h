#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <memory>

#include "Font.h"


/*
    管理字体资源的类，可以通过名字获取字体实例
*/
class FontManager {
public:
    FontManager();
    ~FontManager();

    std::shared_ptr<Font> getFont(const std::string& name) {
        return _fontMap[name];
    }

private:
    std::map<std::string, std::shared_ptr<Font>> _fontMap;
};
