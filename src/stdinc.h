#pragma once

#include <cstdint>
#include <climits>
#include <cmath>
#include <iostream>
#include <cstdarg>
#include <algorithm>
#include <string>
#include <numeric>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <thread>
#include <array>
#include <vector>
#include <functional>
#include <condition_variable>
#include <sys/stat.h>

#ifdef __ANDROID__
# include <SDL_system.h>
#endif

// prevent name clash of WriteFile that comes from SWOS and a Win32 API function
#ifdef _WIN32
# define WriteFile Win32WriteFile
#endif

#include <SimpleIni.h>

#ifdef _WIN32
# undef WriteFile
#endif

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

#if SDL_BYTEORDER != SDL_LIL_ENDIAN
#error "Big endian not supported!"
#endif

constexpr int kRedMask = 0xff;
constexpr int kGreenMask = 0xff00;
constexpr int kBlueMask = 0xff0000;
constexpr int kAlphaMask = 0xff000000;

#include "assert.h"
#include "swos.h"
#include "swossym.h"
#include "log.h"

#include "vm.h"
#define __declspec(naked)

using namespace std::string_literals;

// PTR32: on 32-bit Windows, pointers fit in 32-bit registers and are stored directly.
// On 64-bit or non-Windows, the offset-based VM memory model is used instead.
#if defined(_WIN32) && !defined(_WIN64)
# define PTR32
#endif

#ifndef _WIN32
// Linux/macOS: POSIX equivalents for Windows CRT string functions
#include <strings.h>
#define _stricmp  strcasecmp
#define _strnicmp strncasecmp

// Linux/macOS: provide strncpy_s (Windows CRT function not available on POSIX)
static inline int strncpy_s(char *dest, const char *src, size_t count)
{
    if (!dest || !count) return 0;
    strncpy(dest, src, count);
    dest[count - 1] = '\0';
    return 0;
}
#else
#define strncpy_s(strDest, strSource, size) strncpy_s(strDest, size, strSource, _TRUNCATE)
#endif

#ifndef SWOS_TEST
# define sprintf_s(...) static_assert(false, "sprintf_s detected, use snprintf instead!")
#endif
