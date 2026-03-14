#pragma once

#include "Defs.h"

#include <SDL3/SDL_assert.h>

#include <stdio.h>
#include <time.h>

typedef enum
{
  UIFW_LOG_LEVEL_INFO,
  UIFW_LOG_LEVEL_WARN,
  UIFW_LOG_LEVEL_ERROR,
  UIFW_LOG_LEVEL_FATAL
} uifw_LogLevel;

static inline void uifw_InternalLog(const uifw_LogLevel level, const char *fmt, ...)
{
#ifndef UIFW_DEBUG
  if (level == UIFW_LOG_LEVEL_INFO) {
    return;
  }
#endif

  // Timestamp
  time_t raw;
  struct tm t;
  time(&raw);
  localtime_r(&raw, &t);

  char date_time[32];
  strftime(date_time, sizeof(date_time), "[%I:%M:%S]", &t);

  // Level
  const char *lvlStr = (level == UIFW_LOG_LEVEL_INFO) ? "INFO"
    : (level == UIFW_LOG_LEVEL_WARN)                  ? "WARN"
    : (level == UIFW_LOG_LEVEL_ERROR)                 ? "ERROR"
                                                      : "FATAL";

  // Print
  va_list args = {};
  va_start(args, fmt);
  char msg_buffer[1024];
  vsnprintf(msg_buffer, sizeof(msg_buffer), fmt, args);
  va_end(args);

  printf("%s (%s) : %s\n", date_time, lvlStr, msg_buffer);
}

/* ---- LOG MACROS ---- */
#define ui_Log(level, ...) uifw_InternalLog(level, __VA_ARGS__)

#define ui_LogInfo(...)    uifw_InternalLog(UIFW_LOG_LEVEL_INFO, __VA_ARGS__)
#define ui_LogWarn(...)    uifw_InternalLog(UIFW_LOG_LEVEL_WARN, __VA_ARGS__)
#define ui_LogError(...)   uifw_InternalLog(UIFW_LOG_LEVEL_ERROR, __VA_ARGS__)
#define ui_LogFatal(...)   uifw_InternalLog(UIFW_LOG_LEVEL_FATAL, __VA_ARGS__)
/* -------------------- */

/* ---- DEBUG BREAKPOINT ---- */
#if defined(UIFW_DEBUG)
#define ui_TriggerBreakpoint() SDL_TriggerBreakpoint()
#else
#define ui_TriggerBreakpoint() ((void)0)
#endif
/* -------------------------- */

/* ---- ASSERTIONS ---- */
#if defined(UIFW_DEBUG)
#define ui_Assert(condition, ...)                                                   \
  do {                                                                              \
    if (!(condition)) {                                                             \
      ui_LogFatal(                                                                  \
        "////////////////////////////////////////////////////\n"                    \
        "ASSERT FAILED:\n ├─ File: %s\n ├─ Line: %d\n ├─ Condition: (" #condition   \
        ")\n └─ Message: " __VA_ARGS__,                                             \
        __FILE__, __LINE__);                                                        \
      ui_TriggerBreakpoint();                                                       \
    }                                                                               \
  }                                                                                 \
  while (0)
#else
#define ui_Assert(condition, ...) ((void)0);
#endif
/* -------------------- */
