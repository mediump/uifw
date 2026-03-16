#pragma once

#include <time.h>

#include "uifw/Core/Types.h"

static void ui_Time_init(ui_Time *time)
{
  time->current_ticks = 0;
  time->delta_ticks = 0;
  time->fps = 0;
}

static void ui_Time_recordFrameStart(ui_Time *time)
{
  time->current_ticks = clock();
}

static void ui_Time_recordFrameEnd(ui_Time *time)
{
  time->delta_ticks = clock() - time->current_ticks;

  if (time->delta_ticks > 0) {
    time->fps = CLOCKS_PER_SEC / time->delta_ticks;
  }
}