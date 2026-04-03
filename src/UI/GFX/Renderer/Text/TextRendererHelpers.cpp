#include "TextRendererHelpers.hpp"

#include "TextTypes.hpp"
#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/FontComponents.hpp"
#include "UI/ECS/Components/RenderingComponents.hpp"
#include "UI/ECS/Entity/Entity.hpp"
#include "UI/Utils/StringUtils.hpp"
#include "UI/Widgets/Text/ScrollArea.hpp"

// Space character advance multiplier (fraction of fontSize)
constexpr float SPACE_ADVANCE_MULTIPLIER = 0.25f;

constexpr uint32_t MAX_GLYPH_COUNT = 8192;

#define BORDER_TOP(b)    b.z;
#define BORDER_BOTTOM(b) b.x;
#define BORDER_LEFT(b)   b.w;
#define BORDER_RIGHT(b)  b.y;

// N-pixel slop factors, used to avoid noticeable pops when culling text
#define CULLING_SLOP 5
#define SCROLL_SLOP 30

using namespace ui;

size_t TextRendererHelpers::recordGlyphDrawList(
  const Canvas *canvas, std::vector<FontGlyphInstance> &outInstances)
{
  const auto world = canvas->entity.world();
  const auto textQuery = world.query<ecs::BaseComponent, TextComponent>();

  const auto canvasBounds = canvas->entity.get<ecs::BaseComponent>().rect;

  // Reserve space to avoid reallocations
  outInstances.clear();
  outInstances.reserve(MAX_GLYPH_COUNT);

  size_t counter = 0;

  textQuery.each([&outInstances, &counter, canvasBounds](
                   const ecs::Entity e, const ecs::BaseComponent &baseComponent,
                   const TextComponent &textComponent) {
    if (!baseComponent.visible) {
      return;
    }

    record_text_component(e, baseComponent, textComponent, &outInstances, &counter);
  });

  return counter;
}

void TextRendererHelpers::record_text_component(
  const ecs::Entity e,
  const ecs::BaseComponent &baseComponent,
  const TextComponent &textComponent,
  std::vector<FontGlyphInstance> *outInstances,
  size_t *counter)
{
  // Get parent clipping mask
  Rect clippingMask = baseComponent.rect;
  Vector4f borderRadii = {0.0f, 0.0f, 0.0f, 0.0f};

  const auto currentParent = baseComponent.transformRel.parent;

  if (currentParent != UI_NULL_ENTITY &&
      currentParent.has<ecs::QuadRendererComponent>()) {
    const auto parentBase = currentParent.get<ecs::BaseComponent>();
    const auto quadRenderer = currentParent.get<ecs::QuadRendererComponent>();

    clippingMask = parentBase.rect;
    borderRadii = quadRenderer.borderRadius;

    calculate_final_clipping_mask(&clippingMask, parentBase, textComponent,
                                  currentParent);
  }
  else {
    calculate_final_clipping_mask(&clippingMask, baseComponent, textComponent, e);
  }

  // Font properties
  const FontData *fontData = textComponent.font;

  const auto fontSize = static_cast<float>(textComponent.pixelSize);
  const auto lineHeight =
    fontData->metrics.lineHeight * static_cast<float>(textComponent.pixelSize);

  float availableWidth = baseComponent.rect.width - (textComponent.padding * 2.0f);

  if (textComponent.isScrollable && textComponent.scrollbar != UI_NULL_ENTITY) {
    if (textComponent.scrollbar.get<ecs::BaseComponent>().visible) {
      availableWidth -= ScrollArea::getScrollbarWidth();
    }
  }

  // Calculate all rendered lines and their widths
  const auto textString = std::string(textComponent.text);
  const std::vector<std::string> inputLines = StringUtils::split(textString, "\n");

  // Cache words for each input line to avoid double splits
  std::vector<std::vector<std::string>> cachedWords;
  cachedWords.reserve(inputLines.size());

  // Collect all rendered line info
  struct RenderLine
  {
    size_t index;
    size_t start;
    size_t end;
    float width;
  };

  // TODO: compute line range based on scroll position/projected line height

  std::vector<RenderLine> renderLines;

  for (size_t inputIdx = 0; inputIdx < inputLines.size(); inputIdx++) {
    const auto &line = inputLines[inputIdx];
    std::vector<std::string> words = StringUtils::split(line, " ");
    cachedWords.push_back(std::move(words));

    const std::vector<std::pair<size_t, size_t>> lineWraps =
      calculate_line_wraps(line, textComponent, fontData, availableWidth);
    const std::vector<float> lineWidths =
      calculate_wrapped_line_widths(line, textComponent, fontData, availableWidth);

    for (size_t i = 0; i < lineWraps.size(); i++) {
      renderLines.push_back(
        {inputIdx, lineWraps[i].first, lineWraps[i].second, lineWidths[i]});
    }
  }

  // Calculate vertical alignment offset based on total rendered line count
  const float totalTextHeight = static_cast<float>(renderLines.size()) * lineHeight;

  float verticalOffset = 0.0f;

  switch (textComponent.verticalAlignment) {
  case TextVAlignment_Middle:
    verticalOffset =
      (static_cast<float>(baseComponent.rect.height) - totalTextHeight) / 2.0f;
    break;
  case TextVAlignment_Bottom:
    verticalOffset = static_cast<float>(baseComponent.rect.height) - totalTextHeight;
    break;
  case TextVAlignment_Top:
  default:
    verticalOffset = 0.0f;
    break;
  }

  // Add scroll position to verticalOffset
  const float scrollPos = textComponent.scrollPosition;
  verticalOffset += scrollPos;

  float currentBaselineY = static_cast<float>(baseComponent.rect.y) +
    textComponent.padding + (fontData->metrics.ascender * fontSize) + verticalOffset;

  // Record each rendered line with its own alignment offset
  for (const auto &renderLine : renderLines) {
    // if (currentBaselineY < -SCROLL_SLOP) {
    //   continue;
    // }

    // if (currentBaselineY > clippingMask.height + SCROLL_SLOP) {
    //   break;
    // }
    
    const float lineWidth = renderLine.width;

    // Calculate horizontal alignment offset for this specific rendered line
    float alignmentOffset = 0.0f;
    switch (textComponent.horizontalAlignment) {
    case TextHAlignment_Center:
      alignmentOffset = (availableWidth - lineWidth) / 2.0f;
      break;
    case TextHAlignment_Right:
      alignmentOffset = availableWidth - lineWidth;
      break;
    case TextHAlignment_Left:
    default:
      alignmentOffset = textComponent.padding;
      break;
    }

    record_line(cachedWords[renderLine.index], renderLine.start, renderLine.end, e,
                baseComponent, textComponent, clippingMask, borderRadii, outInstances,
                counter, &currentBaselineY, alignmentOffset);

    currentBaselineY += lineHeight;
  }
}

void TextRendererHelpers::record_line(const std::vector<std::string> &words,
                                      const size_t startIdx,
                                      const size_t endIdx,
                                      const ecs::Entity e,
                                      const ecs::BaseComponent &baseComponent,
                                      const TextComponent &textComponent,
                                      const Rect &clippingMask,
                                      const Vector4f &borderRadii,
                                      std::vector<FontGlyphInstance> *outInstances,
                                      size_t *counter,
                                      const float *currentBaselineY,
                                      const float alignmentOffset)
{
  float currentAdvance = alignmentOffset;

  const float fontSize = textComponent.pixelSize;
  const float spaceWidth = SPACE_ADVANCE_MULTIPLIER * fontSize;

  for (size_t i = startIdx; i < endIdx; i++) {
    const std::string &word = words[i];

    // Record word
    record_word(word, baseComponent, textComponent, clippingMask, borderRadii,
                outInstances, counter, *currentBaselineY, &currentAdvance);

    // Add space after word
    currentAdvance += spaceWidth;
  }
}

void TextRendererHelpers::record_word(const std::string &wordText,
                                      const ecs::BaseComponent &baseComponent,
                                      const TextComponent &textComponent,
                                      const Rect &clippingMask,
                                      const Vector4f &borderRadii,
                                      std::vector<FontGlyphInstance> *outInstances,
                                      size_t *counter,
                                      const float currentBaselineY,
                                      float *currentAdvance)
{
  const FontData *fontData = textComponent.font;

  const auto fontSize = static_cast<float>(textComponent.pixelSize);
  const auto textureSize =
    Vector2f{static_cast<float>(fontData->atlas.atlasDimensions.x),
             static_cast<float>(fontData->atlas.atlasDimensions.y)};

  const char *strPtr = wordText.data();
  size_t strLen = wordText.size();

  while (strLen > 0 && *counter < MAX_GLYPH_COUNT) {
    const uint32_t unicodeValue = SDL_StepUTF8(&strPtr, &strLen);

    // Check if glyph exists in the font atlas
    auto glyphIt = fontData->glyphs.find(unicodeValue);
    if (glyphIt == fontData->glyphs.end()) {
      continue;
    }
    const auto &glyphData = glyphIt->second;

    const float pl = glyphData.planeBounds.left * fontSize;
    const float pt = glyphData.planeBounds.top * fontSize;
    const float pr = glyphData.planeBounds.right * fontSize;
    const float pb = glyphData.planeBounds.bottom * fontSize;

    const float quadWidth = pr - pl;
    const float quadHeight = pt - pb;

    const float atlasHeight = textureSize.y;

    const Vector4f textureCoords = {
      glyphData.atlasBounds.left / textureSize.x,
      (atlasHeight - glyphData.atlasBounds.top) / atlasHeight,
      glyphData.atlasBounds.right / textureSize.x,
      (atlasHeight - glyphData.atlasBounds.bottom) / atlasHeight,
    };

    const float x = static_cast<float>(baseComponent.rect.x) + *currentAdvance + pl;
    const float y = currentBaselineY - pt;

    if (!is_glyph_in_clipping_mask(x, y, quadWidth, quadHeight, clippingMask)) {
      continue;
    }

    outInstances->emplace_back(FontGlyphInstance{
      .position =
        {
          .x = static_cast<float>(baseComponent.rect.x) + *currentAdvance + pl,
          .y = currentBaselineY - pt,
          .z = static_cast<float>(baseComponent.zOrder),
        },
      .size =
        {
          .x = quadWidth,
          .y = quadHeight,
        },
      .textureCoords = textureCoords,
      .color = textComponent.color,
      .parentBounds =
        {
          .x = static_cast<float>(clippingMask.x),
          .y = static_cast<float>(clippingMask.y),
          .z = static_cast<float>(clippingMask.width),
          .w = static_cast<float>(clippingMask.height),
        },
      .parentRadii =
        {
          .x = borderRadii.x,
          .y = borderRadii.y,
          .z = borderRadii.z,
          .w = borderRadii.w,
        },
    });

    *currentAdvance += glyphData.advance * fontSize;
    (*counter)++;
  }
}

std::vector<std::pair<size_t, size_t>> TextRendererHelpers::calculate_line_wraps(
  const std::string &lineText,
  const TextComponent &textComponent,
  const FontData *fontData,
  const float availableWidth)
{
  const float fontSize = textComponent.pixelSize;
  const float spaceWidth = SPACE_ADVANCE_MULTIPLIER * fontSize;

  std::vector<std::pair<size_t, size_t>> lineWraps; // (startWordIndex, endWordIndex)
  float currentAdvance = 0.0f;
  size_t lineStartIndex = 0;

  const std::vector<std::string> words = StringUtils::split(lineText, " ");

  for (size_t i = 0; i < words.size(); i++) {
    const std::string &word = words[i];
    const float wordLength = getWordLength(word, textComponent, fontData);

    // Calculate total width needed for this word (including preceding space if not at
    // line start)
    const float totalWordWidth =
      (currentAdvance > 0.0f) ? (wordLength + spaceWidth) : wordLength;

    // Check if word would overflow the line
    if (i > 0 && textComponent.lineWrapping &&
        currentAdvance + totalWordWidth > availableWidth) {
      // Complete the current line
      lineWraps.emplace_back(lineStartIndex, i);
      lineStartIndex = i;
      currentAdvance = 0.0f;
    }

    currentAdvance += totalWordWidth;
  }

  // Store final line
  lineWraps.emplace_back(lineStartIndex, words.size());

  return lineWraps;
}

std::vector<float> TextRendererHelpers::calculate_wrapped_line_widths(
  const std::string &lineText,
  const TextComponent &textComponent,
  const FontData *fontData,
  const float availableWidth)
{
  const float fontSize = textComponent.pixelSize;
  const float spaceWidth = SPACE_ADVANCE_MULTIPLIER * fontSize;

  std::vector<float> lineWidths;
  float currentAdvance = 0.0f;

  const std::vector<std::string> words = StringUtils::split(lineText, " ");

  for (size_t i = 0; i < words.size(); i++) {
    const std::string &word = words[i];
    const float wordLength = getWordLength(word, textComponent, fontData);

    // Calculate total width needed for this word
    const float totalWordWidth =
      (currentAdvance > 0.0f) ? (wordLength + spaceWidth) : wordLength;

    // Check if word would overflow the line
    if (i > 0 && textComponent.lineWrapping &&
        currentAdvance + totalWordWidth > availableWidth) {
      // Store the width of the completed line (without trailing space)
      lineWidths.emplace_back(currentAdvance - spaceWidth);
      currentAdvance = 0.0f;
    }

    currentAdvance += totalWordWidth;
  }

  // Store final line width
  lineWidths.emplace_back(currentAdvance - spaceWidth);

  return lineWidths;
}

float TextRendererHelpers::getWordLength(const std::string &wordText,
                                         const TextComponent &textComponent,
                                         const FontData *fontData)
{
  float currentAdvance = 0.0f;
  const float fontSize = textComponent.pixelSize;

  const char *strPtr = wordText.data();
  size_t strLen = wordText.size();

  while (strLen > 0) {
    const uint32_t unicodeValue = SDL_StepUTF8(&strPtr, &strLen);
    auto glyphIt = fontData->glyphs.find(unicodeValue);

    if (glyphIt == fontData->glyphs.end()) {
      continue;
    }

    const auto &glyphData = glyphIt->second;
    currentAdvance += glyphData.advance * fontSize;
  }

  return currentAdvance;
}

float TextUtils::computeTotalTextHeight(const TextComponent &textComponent,
                                        const ecs::BaseComponent &baseComponent)
{
  float availableWidth = baseComponent.rect.width;

  if (textComponent.isScrollable && textComponent.scrollbar != UI_NULL_ENTITY) {
    if (textComponent.scrollbar.get<ecs::BaseComponent>().visible) {
      availableWidth -= ScrollArea::getScrollbarWidth();
    }
  }

  const FontData *fontData = textComponent.font;
  const auto fontSize = static_cast<float>(textComponent.pixelSize);
  const auto lineHeight =
    fontData->metrics.lineHeight * static_cast<float>(textComponent.pixelSize);

  const std::string textString(textComponent.text);
  const std::vector<std::string> inputLines = StringUtils::split(textString, "\n");

  size_t totalRenderedLines = 0;

  for (const auto &line : inputLines) {
    if (line.empty()) {
      // Empty line still counts as a rendered line
      totalRenderedLines++;
      continue;
    }

    const std::vector<std::string> words = StringUtils::split(line, " ");

    if (words.empty()) {
      totalRenderedLines++;
      continue;
    }

    if (!textComponent.lineWrapping) {
      // No wrapping: each input line is exactly one rendered line
      totalRenderedLines++;
      continue;
    }

    // Calculate how many lines this input line wraps into
    float currentAdvance = 0.0f;
    const float spaceWidth = SPACE_ADVANCE_MULTIPLIER * fontSize;

    for (size_t i = 0; i < words.size(); i++) {
      const std::string &word = words[i];
      const float wordLength =
        TextRendererHelpers::getWordLength(word, textComponent, fontData);

      const float totalWordWidth =
        (currentAdvance > 0.0f) ? (wordLength + spaceWidth) : wordLength;

      if (i > 0 && currentAdvance + totalWordWidth > availableWidth) {
        // Line wrap occurs
        totalRenderedLines++;
        currentAdvance = 0.0f;
      }

      currentAdvance += totalWordWidth;
    }

    // Count the final line
    totalRenderedLines++;
  }

  return static_cast<float>(totalRenderedLines) * lineHeight;
}

float TextUtils::computeLineWidth(const std::string &text,
                                  const TextComponent &textComponent,
                                  const FontData *fontData)
{
  const float fontSize = static_cast<float>(textComponent.pixelSize);
  const float spaceWidth = SPACE_ADVANCE_MULTIPLIER * fontSize;

  std::vector<std::string> lines = StringUtils::split(text, "\n");
  const std::string &lastLine = lines.back();

  std::vector<std::string> words = StringUtils::split(lastLine, " ");

  if (words.empty() || (words.size() == 1 && words[0].empty())) {
    return 0.0f;
  }

  float currentAdvance = 0.0f;

  for (size_t i = 0; i < words.size(); i++) {
    const float wordLength =
      TextRendererHelpers::getWordLength(words[i], textComponent, fontData);

    currentAdvance += (i > 0) ? (wordLength + spaceWidth) : wordLength;
  }

  return currentAdvance;
}

bool ui::TextRendererHelpers::is_glyph_in_clipping_mask(
  float x, float y, float w, float h, const Rect &clippingMask)
{
  const float a_x1 = x;
  const float a_x2 = x + w;
  const float a_y1 = y;
  const float a_y2 = y + h;

  const float b_x1 = clippingMask.x - CULLING_SLOP;
  const float b_x2 = clippingMask.x + clippingMask.width + CULLING_SLOP;
  const float b_y1 = clippingMask.y - CULLING_SLOP;
  const float b_y2 = clippingMask.y + clippingMask.height + CULLING_SLOP;

  if (a_x1 > b_x2 || b_x1 > a_x2) {
    return false;
  }

  if (a_y1 > b_y2 || b_y1 > a_y2) {
    return false;
  }

  return true;
}

void ui::TextRendererHelpers::calculate_final_clipping_mask(
  Rect *rect,
  const ecs::BaseComponent &baseComponent,
  const TextComponent &textComponent,
  const ecs::Entity &e)
{
  const bool hasScrollbar = textComponent.isScrollable &&
    textComponent.scrollbar != UI_NULL_ENTITY &&
    textComponent.scrollbar.get<ecs::BaseComponent>().visible;

  // Apply border padding
  if (e.has<ecs::QuadRendererComponent>()) {
    const auto quadRenderer = e.get<ecs::QuadRendererComponent>();

    const uint16_t top = BORDER_TOP(quadRenderer.borderWidths);
    const uint16_t bottom = BORDER_BOTTOM(quadRenderer.borderWidths);
    const uint16_t left = BORDER_LEFT(quadRenderer.borderWidths);
    const uint16_t right = BORDER_RIGHT(quadRenderer.borderWidths);

    rect->x += left;

    if (hasScrollbar) {
      rect->width -= left;
    }
    else {
      rect->width -= left + right;
    }

    rect->y += top;
    rect->height -= top + bottom;
  }

  // Apply scrollbar padding (if needed)
  if (hasScrollbar) {
    rect->width -= ScrollArea::getScrollbarWidth();
  }
}
