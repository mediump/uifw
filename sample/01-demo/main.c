#include <stdio.h>

#include "uifw/Core/Utils/Log.h"
#include "uifw/GFX/GFX.h"

int main()
{
  uifw_initGFX();

  constexpr bool TEST_ASSERT = false;
  uifw_Assert(TEST_ASSERT, "Test Assertion Failure");
}