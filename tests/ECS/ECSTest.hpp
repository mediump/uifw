#pragma once

#include <doctest/doctest.h>

#include <UI/ECS/Components/BaseComponent.hpp>
#include <UI/ECS/ECSRoot/ECSRoot.hpp>
#include <UI/ECS/Entity/Entity.hpp>
#include <UI/Layout/LayoutHelpers.hpp>

#include <cstring>

using namespace ui;

TEST_CASE("Defaults")
{
  ecs::ECSRoot root;
  const ecs::Entity entity =
      ecs::createEntity(&root, 0.0f, 0.0f, 256.0f, 256.0f);

  CHECK(entity.has<ecs::BaseComponent>() == true);

  const auto component = entity.get<ecs::BaseComponent>();

  CHECK(component.rect.x == 0.0f);
  CHECK(component.rect.y == 0.0f);
  CHECK(component.rect.width == 256.0f);
  CHECK(component.rect.height == 256.0f);

  CHECK(component.zOrder == 0.0f);
  CHECK(component.inLayout == false);
  CHECK(component.needsUpdate == true);

  const char *entityName = entity.name();
  CHECK(strcmp(entityName, "") == 0);
}

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

  CHECK(entity.has<ecs::BaseComponent>() == true);

  // Get and check data
  const auto component = entity.get<ecs::BaseComponent>();
  CHECK(component.rect.x == X);
  CHECK(component.rect.y == Y);
  CHECK(component.rect.width == WIDTH);
  CHECK(component.rect.height == HEIGHT);

  // Get w/ pointer & reassign
  constexpr float X_2 = X * 2.0f;
  constexpr float Y_2 = Y * 2.0f;
  constexpr float WIDTH_2 = WIDTH * 2.0f;
  constexpr float HEIGHT_2 = HEIGHT * 2.0f;

  auto mutComponent = entity.get_ref<ecs::BaseComponent>();
  mutComponent->rect.x = X_2;
  mutComponent->rect.y = Y_2;
  mutComponent->rect.width = WIDTH_2;
  mutComponent->rect.height = HEIGHT_2;

  CHECK(mutComponent->rect.x == X_2);
  CHECK(mutComponent->rect.y == Y_2);
  CHECK(mutComponent->rect.width == WIDTH_2);
  CHECK(mutComponent->rect.height == HEIGHT_2);

  // Check name
  const char *entityName = entity.name();
  CHECK(strcmp(entityName, NAME) == 0);

  // Remove component
  entity.remove<ecs::BaseComponent>();
  CHECK(entity.has<ecs::BaseComponent>() == false);
}

TEST_CASE("Multiple components")
{
  constexpr float X = 15.0f;
  constexpr float Y = 20.0f;
  constexpr float WIDTH = 250.0f;
  constexpr float HEIGHT = 350.0f;

  const auto NAME_1 = "Entity1";
  const auto NAME_2 = "Entity1";
  const auto NAME_3 = "Entity1";

  ecs::ECSRoot root;

  const ecs::Entity entity1 =
      ecs::createEntity(&root, X, Y, WIDTH, HEIGHT, NAME_1);
  const ecs::Entity entity2 =
      ecs::createEntity(&root, X, Y, WIDTH, HEIGHT, NAME_2);
  const ecs::Entity entity3 =
      ecs::createEntity(&root, X, Y, WIDTH, HEIGHT, NAME_3);

  // Check components
  CHECK(entity1.has<ecs::BaseComponent>() == true);
  CHECK(entity2.has<ecs::BaseComponent>() == true);
  CHECK(entity3.has<ecs::BaseComponent>() == true);

  auto checkValues = [X, Y, WIDTH, HEIGHT](const ecs::Entity &entity) {
    CHECK(entity.has<ecs::BaseComponent>() == true);

    const auto component = entity.get<ecs::BaseComponent>();

    CHECK(component.rect.x == X);
    CHECK(component.rect.y == Y);
    CHECK(component.rect.width == WIDTH);
    CHECK(component.rect.height == HEIGHT);
  };

  checkValues(entity1);
  checkValues(entity2);
  checkValues(entity3);

  // Check names
  const char *entity1Name = entity1.name();
  const char *entity2Name = entity2.name();
  const char *entity3Name = entity3.name();

  CHECK(strcmp(entity1Name, NAME_1) == 0);
  CHECK(strcmp(entity2Name, NAME_2) == 0);
  CHECK(strcmp(entity3Name, NAME_3) == 0);
}

TEST_CASE("Transform hierarchy")
{
  constexpr uint16_t X = 15;
  constexpr uint16_t Y = 20;
  constexpr uint16_t WIDTH = 250;
  constexpr uint16_t HEIGHT = 350;

  const char *PARENT_NAME = "Parent";
  const char *CHILD_1_NAME = "Child1";
  const char *CHILD_2_NAME = "Child2";
  const char *CHILD_3_NAME = "Child3";

  ecs::ECSRoot root;

  const ecs::Entity parent =
      ecs::createEntity(&root, X, Y, WIDTH, HEIGHT, PARENT_NAME);

  ecs::createEntity(&root, X, Y, WIDTH, HEIGHT, CHILD_1_NAME, &parent);
  ecs::createEntity(&root, X, Y, WIDTH, HEIGHT, CHILD_2_NAME, &parent);
  ecs::createEntity(&root, X, Y, WIDTH, HEIGHT, CHILD_3_NAME, &parent);

  // Check parent properties
  const auto parentRel = parent.get<ecs::BaseComponent>().transformRel;

  CHECK(parentRel.nChildren == 3);
  CHECK(parent.name() == PARENT_NAME);

  // Check children properties
  const auto child1Entity = parentRel.first;
  const auto child1Rel =
      child1Entity.get<ecs::BaseComponent>().transformRel;

  CHECK(child1Rel.nChildren == 0);
  CHECK(child1Rel.prev == UI_NULL_ENTITY);
  CHECK_EQ(child1Entity.name(), CHILD_1_NAME);

  const auto child2Entity = child1Rel.next;
  const auto child2Rel =
      child2Entity.get<ecs::BaseComponent>().transformRel;

  CHECK(child2Rel.nChildren == 0);
  CHECK_EQ(child2Rel.prev.name(), child1Entity.name());
  CHECK_EQ(child2Entity.name(), CHILD_2_NAME);

  const auto child3Entity = child2Rel.next;
  const auto child3Rel =
      child3Entity.get<ecs::BaseComponent>().transformRel;

  CHECK(child3Rel.nChildren == 0);
  CHECK_EQ(child3Rel.prev.name(), child2Entity.name());
  CHECK(child3Rel.next == UI_NULL_ENTITY);
  CHECK_EQ(child3Entity.name(), CHILD_3_NAME);
}