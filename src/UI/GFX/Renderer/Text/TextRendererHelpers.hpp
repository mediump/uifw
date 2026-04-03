#pragma once

#include "UI/Canvas/Canvas.hpp"
#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/FontComponents.hpp"
#include "UI/ECS/Entity/Entity.hpp"

#include <vector>

namespace ui {

class TextUtils;

class TextRendererHelpers
{
  friend class TextUtils;

public:
  static size_t recordGlyphDrawList(const Canvas *canvas,
                                    std::vector<FontGlyphInstance> &outInstances);

  static float getWordLength(const std::string &wordText,
                             const TextComponent &textComponent,
                             const FontData *fontData);

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

  static bool is_glyph_in_clipping_mask(
    float x, float y, float w, float h, const Rect &clippingMask);

  static void calculate_final_clipping_mask(Rect *rect,
                                            const ecs::BaseComponent &baseComponent,
                                            const TextComponent &textComponent,
                                            const ecs::Entity &e);
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

  /**
   * Computes the width of the last line of text (useful for caret positioning).
   * Accounts for glyph advances and space characters between words.
   *
   * @param text The text to measure (may contain newlines; measures last line)
   * @param textComponent The text component providing font and style info
   * @param fontData The font data for glyph metrics lookup
   * @return Total width in pixels of the last line
   */
  static float computeLineWidth(const std::string &text,
                                const TextComponent &textComponent,
                                const FontData *fontData);
};

} // namespace ui
