#pragma once

#include "UI/Utils/StringUtils.hpp"

TEST_CASE("[StringUtils] String splitting")
{
  const std::vector<std::string> tokens = StringUtils::split("Line\nBreak\nTest", "\n");

  CHECK_EQ(tokens.size(), 3);
  CHECK_EQ(tokens[0], "Line");
  CHECK_EQ(tokens[1], "Break");
  CHECK_EQ(tokens[2], "Test");
}