#pragma once

#include "UI/Canvas/Canvas.hpp"
#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/FontComponents.hpp"
#include "UI/ECS/Entity/Entity.hpp"

#include <vector>

namespace ui {

class TextRendererHelpers
{
public:
  static size_t recordGlyphDrawList(const Canvas *canvas,
                                       std::vector<FontGlyphInstance> &outInstances);

private:
  static void record_text_component(ecs::Entity e,
                                    const ecs::BaseComponent &baseComponent,
                                    const TextComponent &textComponent,
                                    std::vector<FontGlyphInstance> *outInstances,
                                    size_t *counter);

  static void record_line(const std::vector<std::string> &words,
                          size_t startIdx,
                          size_t endIdx,
                          ecs::Entity e,
                          const ecs::BaseComponent &baseComponent,
                          const TextComponent &textComponent,
                          const Rect &clippingMask,
                          const Vector4f &borderRadii,
                          std::vector<FontGlyphInstance> *outInstances,
                          size_t *counter,
                          const float *currentBaselineY,
                          float alignmentOffset);

  static void record_word(const std::string &wordText,
                          const ecs::BaseComponent &baseComponent,
                          const TextComponent &textComponent,
                          const Rect &clippingMask,
                          const Vector4f &borderRadii,
                          std::vector<FontGlyphInstance> *outInstances,
                          size_t *counter,
                          float currentBaselineY,
                          float *currentAdvance);

  static std::vector<std::pair<size_t, size_t>> calculate_line_wraps(
    const std::string &lineText,
    const TextComponent &textComponent,
    const FontData *fontData,
    float availableWidth);

  static std::vector<float> calculate_wrapped_line_widths(
    const std::string &lineText,
    const TextComponent &textComponent,
    const FontData *fontData,
    float availableWidth);

public:
  static float get_word_length(const std::string &wordText,
                               const TextComponent &textComponent,
                               const FontData *fontData);
};

class TextUtils
{
public:
  /**
   * Computes the total height of all rendered lines for a TextComponent.
   * Takes into account font size, line height, explicit newlines, and word wrapping.
   *
   * @param textComponent The text component to measure
   * @param baseComponent The base component the text is attached to
   * @return Total height in pixels
   */
  static float computeTotalTextHeight(const TextComponent &textComponent,
                                      const ecs::BaseComponent &baseComponent);
};

}