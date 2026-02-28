#include "UI/ECS/Components/BaseComponent.hpp"
#include "UI/ECS/Components/RenderingComponents.hpp"
#include "UI/GFX/Shader.hpp"
#include "UI/Window/Window.hpp"

int main()
{
  ui::initPlatform();

  ui::Window window;
  ui::initializeWindow("SDL3 Window", 1280, 720, &window);

  /* ---------------------------- Setup scene ---------------------------- */
  auto layoutComponent = window.canvas.entity.get_ref<ui::LayoutComponent>();

  layoutComponent->type = ui::LayoutType_Horizontal;
  layoutComponent->margins = { 10, 10, 10, 10 };
  layoutComponent->spacing = 10;

  const auto e1 = ui::ecs::createEntity(&window.ecsRoot, 0, 0, 50, 50,
                                        "Entity1", &window.canvas.entity);

  auto e1Base = e1.get_ref<ui::ecs::BaseComponent>();

  e1Base->minWidth = 500;
  e1Base->maxWidth = 500;

  const auto e2 = ui::ecs::createEntity(&window.ecsRoot, 0, 0, 50, 50,
                                        "Entity2", &window.canvas.entity);

  const auto e3 = ui::ecs::createEntity(&window.ecsRoot, 0, 0, 50, 50,
                                        "Entity3", &window.canvas.entity);
  e3.add<ui::LayoutComponent>();

  auto secondaryLayout = e3.get_ref<ui::LayoutComponent>();

  secondaryLayout->type = ui::LayoutType_Vertical;
  secondaryLayout->spacing = 10;

  const auto e4 =
      ui::ecs::createEntity(&window.ecsRoot, 0, 0, 50, 50, "Entity4", &e3);
  const auto e5 =
      ui::ecs::createEntity(&window.ecsRoot, 0, 0, 50, 50, "Entity5", &e3);
  const auto e6 =
      ui::ecs::createEntity(&window.ecsRoot, 0, 0, 50, 50, "Entity6", &e3);

  auto e6Base = e6.get_ref<ui::ecs::BaseComponent>();

  e6Base->minHeight = 100;
  e6Base->maxHeight = 100;
  e6Base->minWidth = 100;
  e6Base->maxWidth = 100;

  constexpr ui::ecs::Color red = { 1.0, 0.0, 0.0, 1.0 };
  constexpr ui::ecs::Color green = { 0.0, 1.0, 0.0, 1.0 };
  constexpr ui::ecs::Color blue = { 0.0, 0.0, 1.0, 1.0 };
  constexpr ui::ecs::Color white = { 1.0, 1.0, 1.0, 1.0 };

  e1.add<ui::ecs::QuadRenderer>();
  e2.add<ui::ecs::QuadRenderer>();
  e4.add<ui::ecs::QuadRenderer>();
  e5.add<ui::ecs::QuadRenderer>();
  e6.add<ui::ecs::QuadRenderer>();

  e1.get_ref<ui::ecs::QuadRenderer>()->color = red;
  e2.get_ref<ui::ecs::QuadRenderer>()->color = green;
  e4.get_ref<ui::ecs::QuadRenderer>()->color = blue;
  e5.get_ref<ui::ecs::QuadRenderer>()->color = white;
  e6.get_ref<ui::ecs::QuadRenderer>()->color = red;
  /* --------------------------------------------------------------------- */

  while (true) {
    if (!ui::updateWindow(&window)) {
      break;
    }
  }

  return EXIT_SUCCESS;
}
