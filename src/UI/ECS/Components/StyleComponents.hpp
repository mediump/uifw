#pragma once

#include <optional>

#include "UI/GFX/Renderer/RendererTypes.hpp"

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

    return {
      .buttonStyle = std::optional(ButtonStyle{
        .backgroundColor = std::optional(Color4f{0.35f, 0.35f, 0.35f, 1.0f}),
        .borderColor = std::optional(Color4f{0.15f, 0.15f, 0.15f, 1.0f}),
        .textColor = std::optional(Color4f{0.94f, 0.94f, 0.94f, 1.0f}),
        .cornerRadius = std::optional(Vector4i{3, 3, 3, 3}),
      }),
      .buttonStyleHovered = std::optional(ButtonStyle{
        .backgroundColor = std::optional(Color4f{0.30f, 0.30f, 0.30f, 1.0f}),
        .borderColor = std::optional(Color4f{0.15f, 0.15f, 0.15f, 1.0f}),
        .textColor = std::optional(Color4f{0.94f, 0.94f, 0.94f, 1.0f}),
        .cornerRadius = std::optional(Vector4i{3, 3, 3, 3}),
      }),
      .buttonStyleClicked = std::optional(ButtonStyle{
        .backgroundColor = std::optional(Color4f{0.20f, 0.20f, 0.20f, 1.0f}),
        .borderColor = std::optional(Color4f{0.34f, 0.34f, 0.34f, 1.0f}),
        .textColor = std::optional(Color4f{0.94f, 0.94f, 0.94f, 1.0f}),
        .cornerRadius = std::optional(Vector4i{3, 3, 3, 3}),
      }),
    };
  }
};

}