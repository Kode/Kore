#ifndef KORE_GLOBAL_HEADER
#define KORE_GLOBAL_HEADER

/*! \file global.h
    \brief Provides basic functionality that's used all over the place. There's usually no need to manually include this.
*/

#include <stdbool.h>
#include <stdint.h>

#if defined(KORE_PPC)
#define KORE_BIG_ENDIAN
#else
#define KORE_LITTLE_ENDIAN
#endif

#if defined(KORE_PPC)
#define KORE_BIG_ENDIAN
#else
#define KORE_LITTLE_ENDIAN
#endif

#ifdef _MSC_VER
#define KORE_INLINE static __forceinline
#else
#define KORE_INLINE static __attribute__((always_inline)) inline
#endif

#ifdef _MSC_VER
#define KORE_MICROSOFT
#define KORE_MICROSOFT
#endif

#if defined(_WIN32)

#if !defined(KORE_CONSOLE) && !defined(KORE_WINDOWSAPP)
#define KORE_WINDOWS
#endif

#elif defined(__APPLE__)

#include <TargetConditionals.h>

#if TARGET_OS_IPHONE

#if !defined(KORE_TVOS)
#define KORE_IOS
#endif

#define KORE_APPLE_SOC

#else

#define KORE_MACOS

#if defined(__arm64__)
#define KORE_APPLE_SOC
#endif

#endif

#define KORE_POSIX

#elif defined(__linux__)

#if !defined(KORE_ANDROID)
#define KORE_LINUX
#endif

#define KORE_POSIX

#endif

#ifdef KORE_WINDOWS
#if defined(KORE_DYNAMIC)
#define KORE_FUNC __declspec(dllimport)
#define KORE_FUNC __declspec(dllimport)
#elif defined(KORE_DYNAMIC_COMPILE)
#define KORE_FUNC __declspec(dllexport)
#define KORE_FUNC __declspec(dllexport)
#else
#define KORE_FUNC
#define KORE_FUNC
#endif
#else
#define KORE_FUNC
#define KORE_FUNC
#endif

#if defined(__LP64__) || defined(_LP64) || defined(_WIN64)
#define KORE_64
#endif

#ifdef __cplusplus

namespace Kore {
	typedef unsigned char u8;   // 1 Byte
	typedef unsigned short u16; // 2 Byte
	typedef unsigned int u32;   // 4 Byte

#ifdef KORE_WINDOWS
	typedef unsigned __int64 u64; // 8 Byte
#else
	typedef unsigned long long u64;
#endif
	typedef char s8;   // 1 Byte
	typedef short s16; // 2 Byte
	typedef int s32;   // 4 Byte
#ifdef KORE_WINDOWS
	typedef __int64 s64; // 8 Byte
#else
	typedef long long s64;
#endif

	typedef u32 uint; // 4 Byte
	typedef s32 sint; // 4 Byte

#ifdef KORE_64
	typedef s64 spint;
	typedef u64 upint;
#else
	typedef s32 spint;
	typedef u32 upint;
#endif
}

// pseudo C++11
#if !defined(_MSC_VER) && __cplusplus <= 199711L
#define nullptr 0
#define override
#endif

#define Noexcept throw()

#endif

#ifdef __cplusplus
extern "C" {
#endif
int kickstart(int argc, char **argv);
#ifdef __cplusplus
}
#endif

#endif
