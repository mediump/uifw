#pragma once

#include <optional>

#include "UI/GFX/Renderer/RendererTypes.hpp"
#include "UI/GFX/Renderer/Text/TextTypes.hpp"
#include "UI/IO/Text/FontLoader.hpp"

namespace ui {

struct ButtonStyle
{
  std::optional<Color4f> backgroundColor;
  std::optional<Color4f> borderColor;
  std::optional<Color4f> textColor;
  std::optional<Vector4i> cornerRadius;
};

struct AppStyle
{
  FontData *font;
  std::optional<ButtonStyle> buttonStyle;
  std::optional<ButtonStyle> buttonStyleHovered;
  std::optional<ButtonStyle> buttonStyleClicked;
};

class Style
{
public:
  [[nodiscard]] static AppStyle getDefaultAppStyle()
  {
    // TODO: Buttons should support styling with linear gradients
    //
    // TODO: Buttons should have stroke color/width options

    ui::FontData *fontData = FontLoader::loadFont("res/fonts/_generated/Roboto.png",
                                                  "res/fonts/_generated/Roboto.json");

    return {
      .font = fontData,
      .buttonStyle = std::optional(ButtonStyle{
        .backgroundColor = std::optional(Color4f{0.35f, 0.35f, 0.35f, 1.0f}),
        .borderColor = std::optional(Color4f{0.46f, 0.46f, 0.46f, 1.0f}),
        .textColor = std::optional(Color4f{1.0f, 1.0f, 1.0f, 1.0f}),
        .cornerRadius = std::optional(Vector4i{6, 6, 6, 6}),
      }),
      .buttonStyleHovered = std::optional(ButtonStyle{
        .backgroundColor = std::optional(Color4f{1.00f, 0.58f, 0.41f, 1.0f}),
        .borderColor = std::optional(Color4f{0.59f, 0.27f, 0.15f, 1.0f}),
        .textColor = std::optional(Color4f{0.59f, 0.27f, 0.15f, 1.0f}),
        .cornerRadius = std::optional(Vector4i{6, 6, 6, 6}),
      }),
      .buttonStyleClicked = std::optional(ButtonStyle{
        .backgroundColor = std::optional(Color4f{1.00f, 0.58f, 0.41f, 1.0f}),
        .borderColor = std::optional(Color4f{0.59f, 0.27f, 0.15f, 1.0f}),
        .textColor = std::optional(Color4f{0.59f, 0.27f, 0.15f, 1.0f}),
        .cornerRadius = std::optional(Vector4i{6, 6, 6, 6}),
      }),
    };
  }
};

} // namespace ui
