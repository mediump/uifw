#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/RenderingComponents.hpp"
#include "UI/GFX/Renderer/Text/TextHelpers.hpp"
#include "UI/GFX/Shader.hpp"
#include "UI/IO/Text/FontLoader.hpp"
#include "UI/Window/Window.hpp"

#include <chrono>

#include "UI/ECS/Components/InputComponents.hpp"
#include "UI/Layout/LayoutHelpers.hpp"

#define DEMO_WINDOW_WIDTH 1280
#define DEMO_WINDOW_HEIGHT 720

static ui::Color4f lerpColor(const ui::Color4f &a, const ui::Color4f &b, const float t)
{
  return {a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t,
          a.a + (b.a - a.a) * t};
}

int main()
{
  ui::initPlatform();

  ui::Window window;
  ui::initializeWindow("UIFW Window", DEMO_WINDOW_WIDTH, DEMO_WINDOW_HEIGHT, &window);

  ui::FontData fontData = ui::FontLoader::loadFont(
    "res/fonts/_generated/Roboto.png", "res/fonts/_generated/Roboto.json");

  /* ---------------------------- Setup scene ---------------------------- */
  auto layoutComponent = window.canvas.entity.get_ref<ui::LayoutComponent>();

  layoutComponent->type = ui::LayoutType_Horizontal;
  layoutComponent->margins = {10, 10, 10, 10};
  layoutComponent->spacing = 10;

  const auto e1 = ui::ecs::createEntity(&window.ecsRoot, 0, 0, 50, 50, "Entity1",
                                        &window.canvas.entity);

  auto e1Base = e1.get_ref<ui::ecs::BaseComponent>();

  e1Base->minWidth = 500;
  e1Base->maxWidth = 500;

  e1.set<ui::LayoutComponent>({
    .type = ui::LayoutType_Vertical,
    .margins = {10, 10, 10, 10},
    .spacing = 0,
  });

  const auto framerateEntity = ui::TextHelpers::createTextEntity(
    &window.ecsRoot, &fontData, "Framerate: ", {1.0, 1.0, 0.0, 1.0}, 16, 3, 3, 50, 250,
    "FramerateText");

  size_t currentY = 36;

  constexpr ui::Color4f gradientStart = {1.0f, 1.0f, 1.0f, 1.0f};
  constexpr ui::Color4f gradientEnd = {0.5f, 0.5f, 0.5f, 1.0f};

  // for (uint32_t i = 0; i < 20; ++i) {
  //   const std::string name = std::string("TextEntity") + std::to_string(i);
  //   const float t = static_cast<float>(i) / 19.0f;
  //   const ui::Color4f color = lerpColor(gradientStart, gradientEnd, t);
  //
  //   ui::TextHelpers::createTextEntity(&window.ecsRoot, &fontData,
  //                                     "Pack my box with five-dozen liquor jugs.", color,
  //                                     20, 16, currentY, 128, 128, name.c_str());
  //
  //   currentY += 26 * static_cast<uint32_t>(fontData.metrics.lineHeight); // Font size = 20
  // }

  const auto e2 = ui::ecs::createEntity(&window.ecsRoot, 0, 0, 50, 50, "Entity2",
                                        &window.canvas.entity);
  e2.set<ui::LayoutComponent>({
    .type = ui::LayoutType_Vertical,
    .margins = { 5, 5, 5, 5 },
    .spacing = 3
  });

  /* ---- TEXT DISPLAY ---- */
  const auto textDisplayEntity = ui::TextHelpers::createTextEntity(
    &window.ecsRoot, &fontData, "Pack my box with five-dozen liquor jugs.\n!@#$%^&*()_+=",
    {1.0f, 1.0f, 1.0f, 1.0f}, 52, 16, currentY, 128, 128, "BigText", &e2);

  auto textComponent = textDisplayEntity.get_ref<ui::TextComponent>();
  textComponent->horizontalAlignment = ui::TextHAlignment_Center;
  textComponent->verticalAlignment = ui::TextVAlignment_Top;
  /* ---------------------- */

  const auto e3 = ui::ecs::createEntity(&window.ecsRoot, 0, 0, 50, 50, "Entity3",
                                        &window.canvas.entity);
  e3.add<ui::LayoutComponent>();

  auto secondaryLayout = e3.get_ref<ui::LayoutComponent>();

  secondaryLayout->type = ui::LayoutType_Vertical;
  secondaryLayout->spacing = 10;

  const auto e4 = ui::ecs::createEntity(&window.ecsRoot, 0, 0, 50, 50, "Entity4", &e3);
  e4.set<ui::LayoutComponent>({
    .type = ui::LayoutType_Vertical,
    .margins = {5, 5, 5, 5},
    .spacing = 3,
  });

  /* ---------------- Create buttons ---------------- */
  constexpr size_t BUTTON_COUNT = 5;

  std::vector<std::string> buttonEntityNames(BUTTON_COUNT);
  std::vector<std::string> buttonLabels(BUTTON_COUNT);

  for (size_t i = 0; i < BUTTON_COUNT; ++i) {
    const size_t canonicalIndex = i + 1;

    buttonEntityNames[i] = "ButtonEntity" + std::to_string(canonicalIndex);
    buttonLabels[i] = "Button " + std::to_string(canonicalIndex);

    const auto button = ui::ecs::createEntity(&window.ecsRoot, 0, 0, 50, 50,
                                              buttonEntityNames[i].c_str(), &e4);

    button.set<ui::ecs::QuadRendererComponent>({
      .color = {0.36f, 0.36f, 0.36f, 1.0f},
      .borderRadius = {4, 4, 4, 4}
    });
    button.add<ui::ecs::HoverHandlerComponent>();
    button.add<ui::ecs::ButtonComponent>();

    button.set<ui::TextComponent>({
      .text = buttonLabels[i].c_str(),
      .font = &fontData,
      .color = {0.94f, 0.94f, 0.94f, 1.0f},
      .pixelSize = 14,
      .horizontalAlignment = ui::TextHAlignment_Center,
      .verticalAlignment = ui::TextVAlignment_Middle
    });

    auto buttonBaseComponent = button.get_ref<ui::ecs::BaseComponent>();
    buttonBaseComponent->inLayout = true;
    buttonBaseComponent->minHeight = 30;
    buttonBaseComponent->maxHeight = 30;
    buttonBaseComponent->minWidth = 75;
    buttonBaseComponent->maxWidth = 75;
  }

  // Add spacer
  ui::ecs::createEntity(&window.ecsRoot, 0, 0, 50, 50, "Spacer", &e4);
  /* ----------------------------------------------- */

  const auto e5 = ui::ecs::createEntity(&window.ecsRoot, 0, 0, 50, 50, "Entity5", &e3);
  const auto e6 = ui::ecs::createEntity(&window.ecsRoot, 0, 0, 50, 50, "Entity6", &e3);

  auto e6Base = e6.get_ref<ui::ecs::BaseComponent>();

  e6Base->minHeight = 100;
  e6Base->maxHeight = 100;
  e6Base->minWidth = 100;
  e6Base->maxWidth = 100;

  constexpr ui::ecs::Color red = {0.25f, 0.25f, 0.25f, 1.0f};
  constexpr ui::ecs::Color green = {0.1f, 0.1f, 0.1f, 1.0f};
  constexpr ui::ecs::Color blue = {0.75f, 0.75f, 1.75f, 1.0f};
  constexpr ui::ecs::Color white = {0.5f, 0.5f, 0.5f, 1.0f};

  e1.set<ui::ecs::QuadRendererComponent>({
    .color = red,
    .borderRadius = {30, 30, 30, 30},
    .borderColor = {1.0f, 0.0f, 0.0f, 1.0f},
    .borderWidths = {15.0f, 15.0f, 15.0f, 15.0f},
  });
  e2.set<ui::ecs::QuadRendererComponent>({
    .color = green,
    .borderWidths = {0.0f, 5.0f, 0.0f, 5.0f},
  });
  e4.set<ui::ecs::QuadRendererComponent>({
    .color = blue,
    .borderRadius = {9, 9, 9, 9},
    .borderColor = {0.25f, 0.25f, 0.25f, 1.0f},
    .borderWidths = {3.0f, 3.0f, 3.0f, 3.0f}
  });
  e5.add<ui::ecs::QuadRendererComponent>();
  e6.add<ui::ecs::QuadRendererComponent>();
  e5.get_ref<ui::ecs::QuadRendererComponent>()->color = white;
  e6.get_ref<ui::ecs::QuadRendererComponent>()->color = red;
  /* --------------------------------------------------------------------- */

  ui::relayout(&window, DEMO_WINDOW_WIDTH, DEMO_WINDOW_HEIGHT);

  auto framerateTextComponent = framerateEntity.get_ref<ui::TextComponent>();
  auto lastFrameTime = std::chrono::high_resolution_clock::now();

  while (ui::updateWindow(&window)) {
    auto currentFrameTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> deltaTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;

    auto fps = static_cast<uint32_t>(1.0 / deltaTime.count());
    std::string fpsString = "FPS:" + std::to_string(fps);

    framerateTextComponent->text = fpsString.c_str();
  }

  return EXIT_SUCCESS;
}
