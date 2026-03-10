#pragma once

/* ---- PLATFORM DEFINES ---- */
// OS
#ifdef _WIN32
#define UIFW_PLATFORM_WIN32 1
#elif defined(__APPLE__)
#define UIFW_PLATFORM_MACOS 1
#elif defined(__linux__)
#define UIFW_PLATFORM_LINUX 1
#else
#define UIFW_PLATFORM_UNKNOWN 1
#endif

// C Compiler
#if defined(__clang__)
#define UIFW_COMPILER_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__)
#define UIFW_COMPILER_GCC 1
#elif defined(_MSC_VER)
#define UIFW_COMPILER_MSVC 1
#else
#define UIFW_COMPILER_UNKNOWN 1
#endif
/* -------------------------- */

/* ---- DEBUG INFO ---- */
#ifndef _NDEBUG
#define UIFW_DEBUG 1
#endif
/* -------------------- */
