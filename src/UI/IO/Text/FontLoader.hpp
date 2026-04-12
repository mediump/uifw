#pragma once

#include "UI/GFX/Renderer/Text/TextTypes.hpp"

namespace ui {

class FontLoader
{
public:
  [[nodiscard]] static FontData *loadFont(const char *imagePath, const char *jsonPath);

private:
  static void load_JSON_data(const char *jsonPath, FontData *fontData);
};

} // namespace ui
