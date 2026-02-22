#pragma once

#include <doctest/doctest.h>

#include <UI/ECS/ECSRoot/ECSRoot.hpp>
#include <UI/ECS/Entity/Entity.hpp>
#include <UI/ECS/Components/BaseComponents.hpp>

#include <cstring>

using namespace ui;

TEST_CASE("Basic entity")
{
  constexpr float X = 15.0f;
  constexpr float Y = 20.0f;
  constexpr float WIDTH = 250.0f;
  constexpr float HEIGHT = 350.0f;

  const auto NAME = "BasicEntityTest";

  ecs::ECSRoot root;
  const ecs::Entity entity =
      ecs::createEntity(&root, X, Y, WIDTH, HEIGHT, NAME);

  CHECK(entity.has<ecs::BaseComponents>() == true);

  // Get and check data
  const auto component = entity.get<ecs::BaseComponents>();
  CHECK(component.rect.x == X);
  CHECK(component.rect.y == Y);
  CHECK(component.rect.width == WIDTH);
  CHECK(component.rect.height == HEIGHT);

  // Get w/ pointer & reassign
  constexpr float X_2 = X * 2.0f;
  constexpr float Y_2 = Y * 2.0f;
  constexpr float WIDTH_2 = WIDTH * 2.0f;
  constexpr float HEIGHT_2 = HEIGHT * 2.0f;

  auto mutComponent = entity.get_ref<ecs::BaseComponents>();
  mutComponent->rect.x = X_2;
  mutComponent->rect.y = Y_2;
  mutComponent->rect.width = WIDTH_2;
  mutComponent->rect.height = HEIGHT_2;

  CHECK(mutComponent->rect.x == X_2);
  CHECK(mutComponent->rect.y == Y_2);
  CHECK(mutComponent->rect.width == WIDTH_2);
  CHECK(mutComponent->rect.height == HEIGHT_2);

  // Check name
  const char* entityName = entity.name();
  CHECK(strcmp(entityName, NAME) == 0);

  // Remove component
  entity.remove<ecs::BaseComponents>();
  CHECK(entity.has<ecs::BaseComponents>() == false);
}

