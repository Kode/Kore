#ifndef KORE_SIMD_TYPE_CONVERSIONS_HEADER
#define KORE_SIMD_TYPE_CONVERSIONS_HEADER

#include "types.h"
#include <kore3/global.h>
#include <string.h>

/*! \file type_conversions.h
    \brief Provides type casts and type conversions between all 128bit SIMD types
*/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(KORE_SSE2)

// Float32x4 ----> Other
static inline kore_int32x4 kore_float32x4_casto_int32x4(kore_float32x4 t) {
	return _mm_castps_si128(t);
}

static inline kore_uint32x4 kore_float32x4_casto_uint32x4(kore_float32x4 t) {
	return _mm_castps_si128(t);
}

static inline kore_int16x8 kore_float32x4_casto_int16x8(kore_float32x4 t) {
	return _mm_castps_si128(t);
}

static inline kore_uint16x8 kore_float32x4_casto_uint16x8(kore_float32x4 t) {
	return _mm_castps_si128(t);
}

static inline kore_int8x16 kore_float32x4_casto_int8x16(kore_float32x4 t) {
	return _mm_castps_si128(t);
}

static inline kore_uint8x16 kore_float32x4_casto_uint8x16(kore_float32x4 t) {
	return _mm_castps_si128(t);
}

// Int32x4 ----> Other
static inline kore_float32x4 kore_int32x4_casto_float32x4(kore_int32x4 t) {
	return _mm_castsi128_ps(t);
}

static inline kore_uint32x4 kore_int32x4_casto_uint32x4(kore_int32x4 t) {
	// SSE2's m128i is every int type, so we can just return any inbound int type parameter
	return t;
}

static inline kore_int16x8 kore_int32x4_casto_int16x8(kore_int32x4 t) {
	return t;
}

static inline kore_uint16x8 kore_int32x4_casto_uint16x8(kore_int32x4 t) {
	return t;
}

static inline kore_int8x16 kore_int32x4_casto_int8x16(kore_int32x4 t) {
	return t;
}

static inline kore_uint8x16 kore_int32x4_casto_uint8x16(kore_int32x4 t) {
	return t;
}

// Unsigned Int32x4 ----> Other
static inline kore_float32x4 kore_uint32x4_casto_float32x4(kore_uint32x4 t) {
	return _mm_castsi128_ps(t);
}

static inline kore_int32x4 kore_uint32x4_casto_int32x4(kore_uint32x4 t) {
	return t;
}

static inline kore_int16x8 kore_uint32x4_casto_int16x8(kore_uint32x4 t) {
	return t;
}

static inline kore_uint16x8 kore_uint32x4_casto_uint16x8(kore_uint32x4 t) {
	return t;
}

static inline kore_int8x16 kore_uint32x4_casto_int8x16(kore_uint32x4 t) {
	return t;
}

static inline kore_uint8x16 kore_uint32x4_casto_uint8x16(kore_uint32x4 t) {
	return t;
}

// Int16x8 ----> Other
static inline kore_float32x4 kore_int16x8_casto_float32x4(kore_int16x8 t) {
	return _mm_castsi128_ps(t);
}

static inline kore_int32x4 kore_int16x8_casto_int32x4(kore_int16x8 t) {
	return t;
}

static inline kore_uint32x4 kore_int16x8_casto_uint32x4(kore_int16x8 t) {
	return t;
}

static inline kore_uint16x8 kore_int16x8_casto_uint16x8(kore_int16x8 t) {
	return t;
}

static inline kore_int8x16 kore_int16x8_casto_int8x16(kore_int16x8 t) {
	return t;
}

static inline kore_uint8x16 kore_int16x8_casto_uint8x16(kore_int16x8 t) {
	return t;
}

// Unsigned Int16x8 ----> Other
static inline kore_float32x4 kore_uint16x8_casto_float32x4(kore_uint16x8 t) {
	return _mm_castsi128_ps(t);
}

static inline kore_int32x4 kore_uint16x8_casto_int32x4(kore_uint16x8 t) {
	return t;
}

static inline kore_uint32x4 kore_uint16x8_casto_uint32x4(kore_uint16x8 t) {
	return t;
}

static inline kore_int16x8 kore_uint16x8_casto_int16x8(kore_uint16x8 t) {
	return t;
}

static inline kore_int8x16 kore_uint16x8_casto_int8x16(kore_uint16x8 t) {
	return t;
}

static inline kore_uint8x16 kore_uint16x8_casto_uint8x16(kore_uint16x8 t) {
	return t;
}

// Int8x16 ----> Other
static inline kore_float32x4 kore_int8x16_casto_float32x4(kore_int8x16 t) {
	return _mm_castsi128_ps(t);
}

static inline kore_int32x4 kore_int8x16_casto_int32x4(kore_int8x16 t) {
	return t;
}

static inline kore_uint32x4 kore_int8x16_casto_uint32x4(kore_int8x16 t) {
	return t;
}

static inline kore_int16x8 kore_int8x16_casto_int16x8(kore_int8x16 t) {
	return t;
}

static inline kore_uint16x8 kore_int8x16_casto_uint16x8(kore_int8x16 t) {
	return t;
}

static inline kore_uint8x16 kore_int8x16_casto_uint8x16(kore_int8x16 t) {
	return t;
}

// Unsigned Int8x16 ----> Other
static inline kore_float32x4 kore_uint8x16_casto_float32x4(kore_uint8x16 t) {
	return _mm_castsi128_ps(t);
}

static inline kore_int32x4 kore_uint8x16_casto_int32x4(kore_uint8x16 t) {
	return t;
}

static inline kore_uint32x4 kore_uint8x16_casto_uint32x4(kore_uint8x16 t) {
	return t;
}

static inline kore_int16x8 kore_uint8x16_casto_int16x8(kore_uint8x16 t) {
	return t;
}

static inline kore_uint16x8 kore_uint8x16_casto_uint16x8(kore_uint8x16 t) {
	return t;
}

static inline kore_int8x16 kore_uint8x16_casto_int8x16(kore_uint8x16 t) {
	return t;
}

#elif defined(KORE_SSE)

// Float32x4 ----> Other
static inline kore_int32x4 kore_float32x4_casto_int32x4(kore_float32x4 t) {
	float extracted[4];
	_mm_storeu_ps(&extracted[0], t);

	kore_int32x4 cvt;
	memcpy(&cvt.values[0], &extracted[0], sizeof(extracted));

	return cvt;
}

static inline kore_uint32x4 kore_float32x4_casto_uint32x4(kore_float32x4 t) {
	float extracted[4];
	_mm_storeu_ps(&extracted[0], t);

	kore_uint32x4 cvt;
	memcpy(&cvt.values[0], &extracted[0], sizeof(extracted));

	return cvt;
}

static inline kore_int16x8 kore_float32x4_casto_int16x8(kore_float32x4 t) {
	float extracted[4];
	_mm_storeu_ps(&extracted[0], t);

	kore_int16x8 cvt;
	memcpy(&cvt.values[0], &extracted[0], sizeof(extracted));

	return cvt;
}

static inline kore_uint16x8 kore_float32x4_casto_uint16x8(kore_float32x4 t) {
	float extracted[4];
	_mm_storeu_ps(&extracted[0], t);

	kore_uint16x8 cvt;
	memcpy(&cvt.values[0], &extracted[0], sizeof(extracted));

	return cvt;
}

static inline kore_int8x16 kore_float32x4_casto_int8x16(kore_float32x4 t) {
	float extracted[4];
	_mm_storeu_ps(&extracted[0], t);

	kore_int8x16 cvt;
	memcpy(&cvt.values[0], &extracted[0], sizeof(extracted));

	return cvt;
}

static inline kore_uint8x16 kore_float32x4_casto_uint8x16(kore_float32x4 t) {
	float extracted[4];
	_mm_storeu_ps(&extracted[0], t);

	kore_uint8x16 cvt;
	memcpy(&cvt.values[0], &extracted[0], sizeof(extracted));

	return cvt;
}

// Int32x4 ----> Other
static inline kore_float32x4 kore_int32x4_casto_float32x4(kore_int32x4 t) {
	float cvt[4];
	memcpy(&cvt[0], &t.values[0], sizeof(t));

	return _mm_loadu_ps(&cvt[0]);
}

// Unsigned Int32x4 ----> Other
static inline kore_float32x4 kore_uint32x4_casto_float32x4(kore_uint32x4 t) {
	float cvt[4];
	memcpy(&cvt[0], &t.values[0], sizeof(t));

	return _mm_loadu_ps(&cvt[0]);
}

// Int16x8 ----> Other
static inline kore_float32x4 kore_int16x8_casto_float32x4(kore_int16x8 t) {
	float cvt[4];
	memcpy(&cvt[0], &t.values[0], sizeof(t));

	return _mm_loadu_ps(&cvt[0]);
}

// Unsigned Int16x8 ----> Other
static inline kore_float32x4 kore_uint16x8_casto_float32x4(kore_uint16x8 t) {
	float cvt[4];
	memcpy(&cvt[0], &t.values[0], sizeof(t));

	return _mm_loadu_ps(&cvt[0]);
}

// Int8x16 ----> Other
static inline kore_float32x4 kore_int8x16_casto_float32x4(kore_int8x16 t) {
	float cvt[4];
	memcpy(&cvt[0], &t.values[0], sizeof(t));

	return _mm_loadu_ps(&cvt[0]);
}

// Unsigned Int8x16 ----> Other
static inline kore_float32x4 kore_uint8x16_casto_float32x4(kore_uint8x16 t) {
	float cvt[4];
	memcpy(&cvt[0], &t.values[0], sizeof(t));

	return _mm_loadu_ps(&cvt[0]);
}

#elif defined(KORE_NEON)

// Float32x4 ----> Other
static inline kore_int32x4 kore_float32x4_casto_int32x4(kore_float32x4 t) {
	return vreinterpretq_s32_f32(t);
}

static inline kore_uint32x4 kore_float32x4_casto_uint32x4(kore_float32x4 t) {
	return vreinterpretq_u32_f32(t);
}

static inline kore_int16x8 kore_float32x4_casto_int16x8(kore_float32x4 t) {
	return vreinterpretq_s16_f32(t);
}

static inline kore_uint16x8 kore_float32x4_casto_uint16x8(kore_float32x4 t) {
	return vreinterpretq_u16_f32(t);
}

static inline kore_int8x16 kore_float32x4_casto_int8x16(kore_float32x4 t) {
	return vreinterpretq_s8_f32(t);
}

static inline kore_uint8x16 kore_float32x4_casto_uint8x16(kore_float32x4 t) {
	return vreinterpretq_u8_f32(t);
}

// Int32x4 ----> Other
static inline kore_float32x4 kore_int32x4_casto_float32x4(kore_int32x4 t) {
	return vreinterpretq_f32_s32(t);
}

static inline kore_uint32x4 kore_int32x4_casto_uint32x4(kore_int32x4 t) {
	return vreinterpretq_u32_s32(t);
}

static inline kore_int16x8 kore_int32x4_casto_int16x8(kore_int32x4 t) {
	return vreinterpretq_s16_s32(t);
}

static inline kore_uint16x8 kore_int32x4_casto_uint16x8(kore_int32x4 t) {
	return vreinterpretq_u16_s32(t);
}

static inline kore_int8x16 kore_int32x4_casto_int8x16(kore_int32x4 t) {
	return vreinterpretq_s8_s32(t);
}

static inline kore_uint8x16 kore_int32x4_casto_uint8x16(kore_int32x4 t) {
	return vreinterpretq_u8_s32(t);
}

// Unsigned Int32x4 ----> Other
static inline kore_float32x4 kore_uint32x4_casto_float32x4(kore_uint32x4 t) {
	return vreinterpretq_f32_u32(t);
}

static inline kore_int32x4 kore_uint32x4_casto_int32x4(kore_uint32x4 t) {
	return vreinterpretq_s32_u32(t);
}

static inline kore_int16x8 kore_uint32x4_casto_int16x8(kore_uint32x4 t) {
	return vreinterpretq_s16_u32(t);
}

static inline kore_uint16x8 kore_uint32x4_casto_uint16x8(kore_uint32x4 t) {
	return vreinterpretq_u16_u32(t);
}

static inline kore_int8x16 kore_uint32x4_casto_int8x16(kore_uint32x4 t) {
	return vreinterpretq_s8_u32(t);
}

static inline kore_uint8x16 kore_uint32x4_casto_uint8x16(kore_uint32x4 t) {
	return vreinterpretq_u8_u32(t);
}

// Int16x8 ----> Other
static inline kore_float32x4 kore_int16x8_casto_float32x4(kore_int16x8 t) {
	return vreinterpretq_f32_s16(t);
}

static inline kore_int32x4 kore_int16x8_casto_int32x4(kore_int16x8 t) {
	return vreinterpretq_s32_s16(t);
}

static inline kore_uint32x4 kore_int16x8_casto_uint32x4(kore_int16x8 t) {
	return vreinterpretq_u32_s16(t);
}

static inline kore_uint16x8 kore_int16x8_casto_uint16x8(kore_int16x8 t) {
	return vreinterpretq_u16_s16(t);
}

static inline kore_int8x16 kore_int16x8_casto_int8x16(kore_int16x8 t) {
	return vreinterpretq_s8_s16(t);
}

static inline kore_uint8x16 kore_int16x8_casto_uint8x16(kore_int16x8 t) {
	return vreinterpretq_u8_s16(t);
}

// Unsigned Int16x8 ----> Other
static inline kore_float32x4 kore_uint16x8_casto_float32x4(kore_uint16x8 t) {
	return vreinterpretq_f32_u16(t);
}

static inline kore_int32x4 kore_uint16x8_casto_int32x4(kore_uint16x8 t) {
	return vreinterpretq_s32_u16(t);
}

static inline kore_uint32x4 kore_uint16x8_casto_uint32x4(kore_uint16x8 t) {
	return vreinterpretq_u32_u16(t);
}

static inline kore_int16x8 kore_uint16x8_casto_int16x8(kore_uint16x8 t) {
	return vreinterpretq_s16_u16(t);
}

static inline kore_int8x16 kore_uint16x8_casto_int8x16(kore_uint16x8 t) {
	return vreinterpretq_s8_u16(t);
}

static inline kore_uint8x16 kore_uint16x8_casto_uint8x16(kore_uint16x8 t) {
	return vreinterpretq_u8_u16(t);
}

// Int8x16 ----> Other
static inline kore_float32x4 kore_int8x16_casto_float32x4(kore_int8x16 t) {
	return vreinterpretq_f32_s8(t);
}

static inline kore_int32x4 kore_int8x16_casto_int32x4(kore_int8x16 t) {
	return vreinterpretq_s32_s8(t);
}

static inline kore_uint32x4 kore_int8x16_casto_uint32x4(kore_int8x16 t) {
	return vreinterpretq_u32_s8(t);
}

static inline kore_int16x8 kore_int8x16_casto_int16x8(kore_int8x16 t) {
	return vreinterpretq_s16_s8(t);
}

static inline kore_uint16x8 kore_int8x16_casto_uint16x8(kore_int8x16 t) {
	return vreinterpretq_u16_s8(t);
}

static inline kore_uint8x16 kore_int8x16_casto_uint8x16(kore_int8x16 t) {
	return vreinterpretq_u8_s8(t);
}

// Unsigned Int8x16 ----> Other
static inline kore_float32x4 kore_uint8x16_casto_float32x4(kore_uint8x16 t) {
	return vreinterpretq_f32_u8(t);
}

static inline kore_int32x4 kore_uint8x16_casto_int32x4(kore_uint8x16 t) {
	return vreinterpretq_s32_u8(t);
}

static inline kore_uint32x4 kore_uint8x16_casto_uint32x4(kore_uint8x16 t) {
	return vreinterpretq_u32_u8(t);
}

static inline kore_int16x8 kore_uint8x16_casto_int16x8(kore_uint8x16 t) {
	return vreinterpretq_s16_u8(t);
}

static inline kore_uint16x8 kore_uint8x16_casto_uint16x8(kore_uint8x16 t) {
	return vreinterpretq_u16_u8(t);
}

static inline kore_int8x16 kore_uint8x16_casto_int8x16(kore_uint8x16 t) {
	return vreinterpretq_s8_u8(t);
}

// KORE_NOSIMD float fallbacks casts
#else

// Float32x4 ----> Other
static inline kore_int32x4 kore_float32x4_casto_int32x4(kore_float32x4 t) {
	kore_int32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint32x4 kore_float32x4_casto_uint32x4(kore_float32x4 t) {
	kore_uint32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_int16x8 kore_float32x4_casto_int16x8(kore_float32x4 t) {
	kore_int16x8 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint16x8 kore_float32x4_casto_uint16x8(kore_float32x4 t) {
	kore_uint16x8 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_int8x16 kore_float32x4_casto_int8x16(kore_float32x4 t) {
	kore_int8x16 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint8x16 kore_float32x4_casto_uint8x16(kore_float32x4 t) {
	kore_uint8x16 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

// Int32x4 ----> Float32x4
static inline kore_float32x4 kore_int32x4_casto_float32x4(kore_int32x4 t) {
	kore_float32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

// Unsigned Int32x4 ----> Float32x4
static inline kore_float32x4 kore_uint32x4_casto_float32x4(kore_uint32x4 t) {
	kore_float32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

// Int16x8 ----> Float32x4
static inline kore_float32x4 kore_int16x8_casto_float32x4(kore_int16x8 t) {
	kore_float32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

// Unsigned Int16x8 ----> Float32x4
static inline kore_float32x4 kore_uint16x8_casto_float32x4(kore_uint16x8 t) {
	kore_float32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

// Int8x16 ----> Float32x4
static inline kore_float32x4 kore_int8x16_casto_float32x4(kore_int8x16 t) {
	kore_float32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

// Unsigned Int8x16 ----> Float32x4
static inline kore_float32x4 kore_uint8x16_casto_float32x4(kore_uint8x16 t) {
	kore_float32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

#endif // KORE_NOSIMD floats

// Shared signed and unsigned integer vectors for SSE and SIMD-fallback
#if !defined(KORE_SSE2) && (defined(KORE_SSE) || defined(KORE_NOSIMD))

// Int32x4 ----> Other
static inline kore_uint32x4 kore_int32x4_casto_uint32x4(kore_int32x4 t) {
	kore_uint32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_int16x8 kore_int32x4_casto_int16x8(kore_int32x4 t) {
	kore_int16x8 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint16x8 kore_int32x4_casto_uint16x8(kore_int32x4 t) {
	kore_uint16x8 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_int8x16 kore_int32x4_casto_int8x16(kore_int32x4 t) {
	kore_int8x16 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint8x16 kore_int32x4_casto_uint8x16(kore_int32x4 t) {
	kore_uint8x16 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

// Unsigned Int32x4 ----> Other
static inline kore_int32x4 kore_uint32x4_casto_int32x4(kore_uint32x4 t) {
	kore_int32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_int16x8 kore_uint32x4_casto_int16x8(kore_uint32x4 t) {
	kore_int16x8 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint16x8 kore_uint32x4_casto_uint16x8(kore_uint32x4 t) {
	kore_uint16x8 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_int8x16 kore_uint32x4_casto_int8x16(kore_uint32x4 t) {
	kore_int8x16 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint8x16 kore_uint32x4_casto_uint8x16(kore_uint32x4 t) {
	kore_uint8x16 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

// Int16x8 ----> Other
static inline kore_int32x4 kore_int16x8_casto_int32x4(kore_int16x8 t) {
	kore_int32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint32x4 kore_int16x8_casto_uint32x4(kore_int16x8 t) {
	kore_uint32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint16x8 kore_int16x8_casto_uint16x8(kore_int16x8 t) {
	kore_uint16x8 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_int8x16 kore_int16x8_casto_int8x16(kore_int16x8 t) {
	kore_int8x16 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint8x16 kore_int16x8_casto_uint8x16(kore_int16x8 t) {
	kore_uint8x16 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

// Unsigned Int16x8 ----> Other
static inline kore_int32x4 kore_uint16x8_casto_int32x4(kore_uint16x8 t) {
	kore_int32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint32x4 kore_uint16x8_casto_uint32x4(kore_uint16x8 t) {
	kore_uint32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_int16x8 kore_uint16x8_casto_int16x8(kore_uint16x8 t) {
	kore_int16x8 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_int8x16 kore_uint16x8_casto_int8x16(kore_uint16x8 t) {
	kore_int8x16 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint8x16 kore_uint16x8_casto_uint8x16(kore_uint16x8 t) {
	kore_uint8x16 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

// Int8x16 ----> Other
static inline kore_int32x4 kore_int8x16_casto_int32x4(kore_int8x16 t) {
	kore_int32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint32x4 kore_int8x16_casto_uint32x4(kore_int8x16 t) {
	kore_uint32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_int16x8 kore_int8x16_casto_int16x8(kore_int8x16 t) {
	kore_int16x8 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint16x8 kore_int8x16_casto_uint16x8(kore_int8x16 t) {
	kore_uint16x8 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint8x16 kore_int8x16_casto_uint8x16(kore_int8x16 t) {
	kore_uint8x16 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

// Unsigned Int8x16 ----> Other
static inline kore_int32x4 kore_uint8x16_casto_int32x4(kore_uint8x16 t) {
	kore_int32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint32x4 kore_uint8x16_casto_uint32x4(kore_uint8x16 t) {
	kore_uint32x4 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_int16x8 kore_uint8x16_casto_int16x8(kore_uint8x16 t) {
	kore_int16x8 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_uint16x8 kore_uint8x16_casto_uint16x8(kore_uint8x16 t) {
	kore_uint16x8 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

static inline kore_int8x16 kore_uint8x16_casto_int8x16(kore_uint8x16 t) {
	kore_int8x16 cvt;
	memcpy(&cvt.values[0], &t.values[0], sizeof(t));

	return cvt;
}

#endif // KORE_SSE || KORE_NOSIMD

#ifdef __cplusplus
}
#endif

#endif
