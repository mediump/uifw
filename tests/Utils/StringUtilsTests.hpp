#pragma once

#include "UI/Utils/StringUtils.hpp"

TEST_CASE("[StringUtils] String splitting")
{
  const std::vector<std::string> tokens = StringUtils::split("Line\nBreak\nTest", "\n");

  CHECK_EQ(tokens.size(), 3);
  CHECK_EQ(tokens[0], "Line");
  CHECK_EQ(tokens[1], "Break");
  CHECK_EQ(tokens[2], "Test");

  const std::vector<std::string> tokens2 = StringUtils::split("Line Space Test", " ");

  CHECK_EQ(tokens2.size(), 3);
  CHECK_EQ(tokens2[0], "Line");
  CHECK_EQ(tokens2[1], "Space");
  CHECK_EQ(tokens2[2], "Test");
}