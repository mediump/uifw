#pragma once

#include "UI/Canvas/Canvas.hpp"
#include "UI/Window/Window.hpp"

#include <doctest/doctest.h>

#include "UI/ECS/Components/BaseComponent.hpp"

TEST_CASE("Construct window with canvas")
{
  constexpr int WINDOW_WIDTH = 1280;
  constexpr int WINDOW_HEIGHT = 720;

  ui::initPlatform();

  ui::Window window;
  ui::initializeWindow("Test", WINDOW_WIDTH, WINDOW_HEIGHT, &window);

  ui::Canvas &canvas = window.canvas;

  CHECK(canvas.entity.has<ui::LayoutComponent>());
  CHECK(canvas.entity.name() == "Canvas");

  auto baseComponent = canvas.entity.get<ui::ecs::BaseComponent>();

  CHECK(baseComponent.rect.width == WINDOW_WIDTH);
  CHECK(baseComponent.rect.height == WINDOW_HEIGHT);

  auto layout = canvas.entity.get<ui::LayoutComponent>();

  CHECK(layout.type == ui::LayoutType_Vertical);

  CHECK(layout.margins.top == 0.0f);
  CHECK(layout.margins.right == 0.0f);
  CHECK(layout.margins.bottom == 0.0f);
  CHECK(layout.margins.left == 0.0f);

  CHECK(layout.spacing == 0.0f);
}

TEST_CASE("Basic horizontal layout")
{
  constexpr int WINDOW_WIDTH = 180;
  constexpr int WINDOW_HEIGHT = 110;

  constexpr uint16_t X = 15;
  constexpr uint16_t Y = 20;
  constexpr uint16_t WIDTH = 250;
  constexpr uint16_t HEIGHT = 350;

  const char *CHILD_1_NAME = "Child1";
  const char *CHILD_2_NAME = "Child2";
  const char *CHILD_3_NAME = "Child3";

  ui::initPlatform();

  ui::Window window;
  ui::initializeWindow("Test", WINDOW_WIDTH, WINDOW_HEIGHT, &window);

  ui::Canvas &canvas = window.canvas;
  CHECK(canvas.entity.has<ui::LayoutComponent>());
  CHECK(canvas.entity.name() == "Canvas");

  auto canvasLayout = canvas.entity.get_ref<ui::LayoutComponent>();

  canvasLayout->type = LayoutType_Horizontal;
  canvasLayout->margins = {5, 5, 5, 5};
  canvasLayout->spacing = 10;

  auto child1 = ecs::createEntity(&window.ecsRoot, X, Y, WIDTH, HEIGHT,
                                  CHILD_1_NAME, &canvas.entity);
  auto child2 = ecs::createEntity(&window.ecsRoot, X, Y, WIDTH, HEIGHT,
                                  CHILD_2_NAME, &canvas.entity);
  auto child3 = ecs::createEntity(&window.ecsRoot, X, Y, WIDTH, HEIGHT,
                                  CHILD_3_NAME, &canvas.entity);

  CHECK(canvas.entity.get<ecs::BaseComponent>().transformRel.nChildren == 3);

  // Apply layout
  ui::layoutChildren(canvas.entity);

  auto validateComponent = [](ecs::Entity entity) {
    const auto baseComponent = entity.get<ui::ecs::BaseComponent>();
    CHECK(baseComponent.rect.width == 50);
    CHECK(baseComponent.rect.height == 100);
  };

  validateComponent(child1);
  validateComponent(child2);
  validateComponent(child3);
}

TEST_CASE("Horizontal layout with constraint")
{
  constexpr int WINDOW_WIDTH = 180;
  constexpr int WINDOW_HEIGHT = 110;

  constexpr uint16_t X = 15.0f;
  constexpr uint16_t Y = 20.0f;
  constexpr uint16_t WIDTH = 250.0f;
  constexpr uint16_t HEIGHT = 350.0f;

  const char *CHILD_1_NAME = "Child1";
  const char *CHILD_2_NAME = "Child2";
  const char *CHILD_3_NAME = "Child3";

  ui::initPlatform();

  ui::Window window;
  ui::initializeWindow("Test", WINDOW_WIDTH, WINDOW_HEIGHT, &window);

  ui::Canvas &canvas = window.canvas;
  CHECK(canvas.entity.has<ui::LayoutComponent>());
  CHECK(canvas.entity.name() == "Canvas");

  auto canvasLayout = canvas.entity.get_ref<ui::LayoutComponent>();

  canvasLayout->type = LayoutType_Horizontal;
  canvasLayout->margins = {5, 5, 5, 5};
  canvasLayout->spacing = 10.0f;

  auto child1 = ecs::createEntity(&window.ecsRoot, X, Y, WIDTH, HEIGHT,
                                  CHILD_1_NAME, &canvas.entity);

  // Constrain child1 width to 100px
  auto child1BaseComponent = child1.get_ref<ui::ecs::BaseComponent>();

  child1BaseComponent->minWidth = 100.0f;
  child1BaseComponent->minHeight = 100.0f;
  // ----

  auto child2 = ecs::createEntity(&window.ecsRoot, X, Y, WIDTH, HEIGHT,
                                  CHILD_2_NAME, &canvas.entity);
  auto child3 = ecs::createEntity(&window.ecsRoot, X, Y, WIDTH, HEIGHT,
                                  CHILD_3_NAME, &canvas.entity);

  CHECK(canvas.entity.get<ecs::BaseComponent>().transformRel.nChildren == 3);

  // Apply layout
  ui::layoutChildren(canvas.entity);

  auto validateComponent = [](ecs::Entity entity, uint16_t expectedWidth) {
    const auto baseComponent = entity.get<ui::ecs::BaseComponent>();
    CHECK(baseComponent.rect.width == expectedWidth);
    CHECK(baseComponent.rect.height == 100);
  };

  validateComponent(child1, 100);
  validateComponent(child2, 25);
  validateComponent(child3, 25);
}

TEST_CASE("Basic vertical layout")
{
  constexpr int WINDOW_WIDTH = 110;
  constexpr int WINDOW_HEIGHT = 180;

  constexpr uint16_t X = 15;
  constexpr uint16_t Y = 20;
  constexpr uint16_t WIDTH = 250;
  constexpr uint16_t HEIGHT = 350;

  const char *CHILD_1_NAME = "Child1";
  const char *CHILD_2_NAME = "Child2";
  const char *CHILD_3_NAME = "Child3";

  ui::initPlatform();

  ui::Window window;
  ui::initializeWindow("Test", WINDOW_WIDTH, WINDOW_HEIGHT, &window);

  ui::Canvas &canvas = window.canvas;
  CHECK(canvas.entity.has<ui::LayoutComponent>());
  CHECK(canvas.entity.name() == "Canvas");

  auto canvasLayout = canvas.entity.get_ref<ui::LayoutComponent>();

  canvasLayout->type = LayoutType_Vertical;
  canvasLayout->margins = {5, 5, 5, 5};
  canvasLayout->spacing = 10;

  auto child1 = ecs::createEntity(&window.ecsRoot, X, Y, WIDTH, HEIGHT,
                                  CHILD_1_NAME, &canvas.entity);
  auto child2 = ecs::createEntity(&window.ecsRoot, X, Y, WIDTH, HEIGHT,
                                  CHILD_2_NAME, &canvas.entity);
  auto child3 = ecs::createEntity(&window.ecsRoot, X, Y, WIDTH, HEIGHT,
                                  CHILD_3_NAME, &canvas.entity);

  CHECK(canvas.entity.get<ecs::BaseComponent>().transformRel.nChildren == 3);

  // Apply layout
  ui::layoutChildren(canvas.entity);

  auto validateComponent = [](ecs::Entity entity) {
    const auto baseComponent = entity.get<ui::ecs::BaseComponent>();
    CHECK(baseComponent.rect.width == 100);
    CHECK(baseComponent.rect.height == 50);
  };

  validateComponent(child1);
  validateComponent(child2);
  validateComponent(child3);
}

TEST_CASE("Vertical layout with constraint")
{
  constexpr int WINDOW_WIDTH = 110;
  constexpr int WINDOW_HEIGHT = 180;

  constexpr uint16_t X = 15.0f;
  constexpr uint16_t Y = 20.0f;
  constexpr uint16_t WIDTH = 250.0f;
  constexpr uint16_t HEIGHT = 350.0f;

  const char *CHILD_1_NAME = "Child1";
  const char *CHILD_2_NAME = "Child2";
  const char *CHILD_3_NAME = "Child3";

  ui::initPlatform();

  ui::Window window;
  ui::initializeWindow("Test", WINDOW_WIDTH, WINDOW_HEIGHT, &window);

  ui::Canvas &canvas = window.canvas;
  CHECK(canvas.entity.has<ui::LayoutComponent>());
  CHECK(canvas.entity.name() == "Canvas");

  auto canvasLayout = canvas.entity.get_ref<ui::LayoutComponent>();

  canvasLayout->type = LayoutType_Vertical;
  canvasLayout->margins = {5, 5, 5, 5};
  canvasLayout->spacing = 10.0f;

  auto child1 = ecs::createEntity(&window.ecsRoot, X, Y, WIDTH, HEIGHT,
                                  CHILD_1_NAME, &canvas.entity);

  // Constrain child1 width to 100px
  auto child1BaseComponent = child1.get_ref<ui::ecs::BaseComponent>();

  child1BaseComponent->minHeight = 100.0f;
  child1BaseComponent->maxHeight = 100.0f;
  // ----

  auto child2 = ecs::createEntity(&window.ecsRoot, X, Y, WIDTH, HEIGHT,
                                  CHILD_2_NAME, &canvas.entity);
  auto child3 = ecs::createEntity(&window.ecsRoot, X, Y, WIDTH, HEIGHT,
                                  CHILD_3_NAME, &canvas.entity);

  CHECK(canvas.entity.get<ecs::BaseComponent>().transformRel.nChildren == 3);

  // Apply layout
  ui::layoutChildren(canvas.entity);

  auto validateComponent = [](ecs::Entity entity, uint16_t expectedHeight) {
    const auto baseComponent = entity.get<ui::ecs::BaseComponent>();
    CHECK(baseComponent.rect.width == 100);
    CHECK(baseComponent.rect.height == expectedHeight);
  };

  validateComponent(child1, 100);
  validateComponent(child2, 25);
  validateComponent(child3, 25);
}