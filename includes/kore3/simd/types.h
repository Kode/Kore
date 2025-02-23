#ifndef KORE_SIMD_TYPES_HEADER
#define KORE_SIMD_TYPES_HEADER

#include <kore3/global.h>

/*! \file types.h
    \brief Provides 128bit SIMD types which are mapped to equivalent SSE or Neon types.
*/

#ifdef __cplusplus
extern "C" {
#endif

// Any level of AVX Capability (Could be AVX, AVX2, AVX512, etc.)
//(Currently) only used for checking existence of earlier SSE instruction sets
#if defined(__AVX__)
// Unfortunate situation here
// MSVC does not provide compiletime macros for the following instruction sets
// but their existence is implied by AVX and higher
#define KORE_SSE4_2
#define KORE_SSE4_1
#define KORE_SSSE3
#define KORE_SSE3
#endif

// SSE2 Capability check
// Note for Windows:
//	_M_IX86_FP checks SSE2 and SSE for 32bit Windows programs only, and is unset if not a 32bit program.
//	SSE2 and earlier is --guaranteed-- to be active for any 64bit Windows program
#if defined(__SSE2__) || (_M_IX86_FP == 2) || (defined(KINC_WINDOWS) && defined(KINC_64))
#define KORE_SSE2
#endif

// SSE Capability check
#if defined(__SSE__) || _M_IX86_FP == 2 || _M_IX86_FP == 1 || (defined(KINC_WINDOWS) && !defined(__aarch64__)) ||                                              \
    (defined(KINC_WINDOWSAPP) && !defined(__aarch64__)) || (defined(KINC_MACOS) && __x86_64)

#define KORE_SSE
#endif

// NEON Capability check
#if defined(KINC_IOS) || defined(KINC_SWITCH) || defined(__aarch64__) || defined(KINC_NEON)
#define KORE_NEON
#endif

// No SIMD Capabilities
#if !defined(KORE_SSE4_2) && !defined(KORE_SSE4_1) && !defined(KORE_SSSE3) && !defined(KORE_SSE3) && !defined(KORE_SSE2) && !defined(KORE_SSE) &&              \
    !defined(KORE_NEON)

#define KORE_NOSIMD
#endif

#define KORE_SHUFFLE_TABLE(LANE_A1, LANE_A2, LANE_B1, LANE_B2)                                                                                                 \
	((((LANE_B2) & 0x3) << 6) | (((LANE_B1) & 0x3) << 4) | (((LANE_A2) & 0x3) << 2) | (((LANE_A1) & 0x3) << 0))

#if defined(KORE_SSE2)

// SSE_## related headers include earlier revisions, IE
// SSE2 contains all of SSE
#include <emmintrin.h>

typedef __m128 kore_float32x4;
typedef __m128 kore_float32x4_mask;

typedef __m128i kore_int8x16;
typedef __m128i kore_int8x16_mask;
typedef __m128i kore_uint8x16;
typedef __m128i kore_uint8x16_mask;
typedef __m128i kore_int16x8;
typedef __m128i kore_int16x8_mask;
typedef __m128i kore_uint16x8;
typedef __m128i kore_uint16x8_mask;
typedef __m128i kore_int32x4;
typedef __m128i kore_int32x4_mask;
typedef __m128i kore_uint32x4;
typedef __m128i kore_uint32x4_mask;

#elif defined(KORE_SSE)

#include <xmmintrin.h>

typedef __m128 kore_float32x4;
typedef __m128 kore_float32x4_mask;

typedef struct kore_int8x16 {
	int8_t values[16];
} kore_int8x16;

typedef struct kore_uint8x16 {
	uint8_t values[16];
} kore_uint8x16;

typedef struct kore_int16x8 {
	int16_t values[8];
} kore_int16x8;

typedef struct kore_uint16x8 {
	uint16_t values[8];
} kore_uint16x8;

typedef struct kore_int32x4 {
	int32_t values[4];
} kore_int32x4;

typedef struct kore_uint32x4 {
	uint32_t values[4];
} kore_uint32x4;

typedef kore_int8x16 kore_int8x16_mask;
typedef kore_uint8x16 kore_uint8x16_mask;
typedef kore_int16x8 kore_int16x8_mask;
typedef kore_uint16x8 kore_uint16x8_mask;
typedef kore_int32x4 kore_int32x4_mask;
typedef kore_uint32x4 kore_uint32x4_mask;

#elif defined(KORE_NEON)

#include <arm_neon.h>

typedef float32x4_t kore_float32x4;
typedef uint32x4_t kore_float32x4_mask;

typedef int8x16_t kore_int8x16;
typedef uint8x16_t kore_int8x16_mask;
typedef uint8x16_t kore_uint8x16;
typedef uint8x16_t kore_uint8x16_mask;
typedef int16x8_t kore_int16x8;
typedef uint16x8_t kore_int16x8_mask;
typedef uint16x8_t kore_uint16x8;
typedef uint16x8_t kore_uint16x8_mask;
typedef int32x4_t kore_int32x4;
typedef uint32x4_t kore_int32x4_mask;
typedef uint32x4_t kore_uint32x4;
typedef uint32x4_t kore_uint32x4_mask;

#else

#include <kore3/math/core.h>

typedef struct kore_float32x4 {
	float values[4];
} kore_float32x4;

typedef kore_float32x4 kore_float32x4_mask;

typedef struct kore_int8x16 {
	int8_t values[16];
} kore_int8x16;

typedef struct kore_uint8x16 {
	uint8_t values[16];
} kore_uint8x16;

typedef struct kore_int16x8 {
	int16_t values[8];
} kore_int16x8;

typedef struct kore_uint16x8 {
	uint16_t values[8];
} kore_uint16x8;

typedef struct kore_int32x4 {
	int32_t values[4];
} kore_int32x4;

typedef struct kore_uint32x4 {
	uint32_t values[4];
} kore_uint32x4;

typedef kore_int8x16 kore_int8x16_mask;
typedef kore_uint8x16 kore_uint8x16_mask;
typedef kore_int16x8 kore_int16x8_mask;
typedef kore_uint16x8 kore_uint16x8_mask;
typedef kore_int32x4 kore_int32x4_mask;
typedef kore_uint32x4 kore_uint32x4_mask;

#endif

#ifdef __cplusplus
}
#endif

#endif
