#ifndef KORE_SIMD_FLOAT32X4_HEADER
#define KORE_SIMD_FLOAT32X4_HEADER

#include "types.h"
#include <string.h>

/*! \file float32x4.h
    \brief Provides 128bit four-element floating point SIMD operations which are mapped to equivalent SSE or Neon operations.
*/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(KORE_SSE)

static inline kore_float32x4 kore_float32x4_load_int32x4(kore_int32x4 value) {
	return _mm_cvtepi32_ps(value);
}

static inline kore_float32x4 kore_float32x4_load_uint32x4(kore_uint32x4 value) {
	return _mm_cvtepi32_ps(value);
}

static inline kore_float32x4 kore_float32x4_intrin_load(const float *values) {
	return _mm_load_ps(values);
}

static inline kore_float32x4 kore_float32x4_intrin_load_unaligned(const float *values) {
	return _mm_loadu_ps(values);
}

static inline kore_float32x4 kore_float32x4_load(float a, float b, float c, float d) {
	return _mm_set_ps(d, c, b, a);
}

static inline kore_float32x4 kore_float32x4_load_all(float t) {
	return _mm_set_ps1(t);
}

static inline void kore_float32x4_store(float *destination, kore_float32x4 value) {
	_mm_store_ps(destination, value);
}

static inline void kore_float32x4_store_unaligned(float *destination, kore_float32x4 value) {
	_mm_storeu_ps(destination, value);
}

static inline float kore_float32x4_get(kore_float32x4 t, int index) {
	union {
		__m128 value;
		float elements[4];
	} converter;
	converter.value = t;
	return converter.elements[index];
}

static inline kore_float32x4 kore_float32x4_abs(kore_float32x4 t) {
	__m128 mask = _mm_set_ps1(-0.f);
	return _mm_andnot_ps(mask, t);
}

static inline kore_float32x4 kore_float32x4_add(kore_float32x4 a, kore_float32x4 b) {
	return _mm_add_ps(a, b);
}

static inline kore_float32x4 kore_float32x4_div(kore_float32x4 a, kore_float32x4 b) {
	return _mm_div_ps(a, b);
}

static inline kore_float32x4 kore_float32x4_mul(kore_float32x4 a, kore_float32x4 b) {
	return _mm_mul_ps(a, b);
}

static inline kore_float32x4 kore_float32x4_neg(kore_float32x4 t) {
	__m128 negative = _mm_set_ps1(-1.0f);
	return _mm_mul_ps(t, negative);
}

static inline kore_float32x4 kore_float32x4_reciprocal_approximation(kore_float32x4 t) {
	return _mm_rcp_ps(t);
}

static inline kore_float32x4 kore_float32x4_reciprocal_sqrt_approximation(kore_float32x4 t) {
	return _mm_rsqrt_ps(t);
}

static inline kore_float32x4 kore_float32x4_sub(kore_float32x4 a, kore_float32x4 b) {
	return _mm_sub_ps(a, b);
}

static inline kore_float32x4 kore_float32x4_sqrt(kore_float32x4 t) {
	return _mm_sqrt_ps(t);
}

static inline kore_float32x4 kore_float32x4_max(kore_float32x4 a, kore_float32x4 b) {
	return _mm_max_ps(a, b);
}

static inline kore_float32x4 kore_float32x4_min(kore_float32x4 a, kore_float32x4 b) {
	return _mm_min_ps(a, b);
}

static inline kore_float32x4_mask kore_float32x4_cmpeq(kore_float32x4 a, kore_float32x4 b) {
	return _mm_cmpeq_ps(a, b);
}

static inline kore_float32x4_mask kore_float32x4_cmpge(kore_float32x4 a, kore_float32x4 b) {
	return _mm_cmpge_ps(a, b);
}

static inline kore_float32x4_mask kore_float32x4_cmpgt(kore_float32x4 a, kore_float32x4 b) {
	return _mm_cmpgt_ps(a, b);
}

static inline kore_float32x4_mask kore_float32x4_cmple(kore_float32x4 a, kore_float32x4 b) {
	return _mm_cmple_ps(a, b);
}

static inline kore_float32x4_mask kore_float32x4_cmplt(kore_float32x4 a, kore_float32x4 b) {
	return _mm_cmplt_ps(a, b);
}

static inline kore_float32x4_mask kore_float32x4_cmpneq(kore_float32x4 a, kore_float32x4 b) {
	return _mm_cmpneq_ps(a, b);
}

static inline kore_float32x4 kore_float32x4_sel(kore_float32x4 a, kore_float32x4 b, kore_float32x4_mask mask) {
	return _mm_xor_ps(b, _mm_and_ps(mask, _mm_xor_ps(a, b)));
}

static inline kore_float32x4 kore_float32x4_or(kore_float32x4 a, kore_float32x4 b) {
	return _mm_or_ps(a, b);
}

static inline kore_float32x4 kore_float32x4_and(kore_float32x4 a, kore_float32x4 b) {
	return _mm_and_ps(a, b);
}

static inline kore_float32x4 kore_float32x4_xor(kore_float32x4 a, kore_float32x4 b) {
	return _mm_xor_ps(a, b);
}

static inline kore_float32x4 kore_float32x4_not(kore_float32x4 t) {
	__m128 zeroes = _mm_setzero_ps();
	return _mm_xor_ps(t, _mm_cmpeq_ps(zeroes, zeroes));
}

#define kore_float32x4_shuffle_custom(abcd, efgh, left_1, left_2, right_1, right_2)                                                                            \
	_mm_shuffle_ps((abcd), (efgh), KORE_SHUFFLE_TABLE((left_1), (left_2), (right_1), (right_2)))

static inline kore_float32x4 kore_float32x4_shuffle_aebf(kore_float32x4 abcd, kore_float32x4 efgh) {
	// aka unpacklo aka zip1 aka interleave low
	return _mm_unpacklo_ps(abcd, efgh);
}

static inline kore_float32x4 kore_float32x4_shuffle_cgdh(kore_float32x4 abcd, kore_float32x4 efgh) {
	// aka unpackhi aka zip2 aka interleave high
	return _mm_unpackhi_ps(abcd, efgh);
}

static inline kore_float32x4 kore_float32x4_shuffle_abef(kore_float32x4 abcd, kore_float32x4 efgh) {
	// aka movelh
	return _mm_movelh_ps(abcd, efgh);
}

static inline kore_float32x4 kore_float32x4_shuffle_ghcd(kore_float32x4 abcd, kore_float32x4 efgh) {
	// aka movehl
	return _mm_movehl_ps(abcd, efgh);
}

#elif defined(KORE_NEON)

static inline kore_float32x4 kore_float32x4_intrin_load(const float *values) {
	return vld1q_f32(values);
}

static inline kore_float32x4 kore_float32x4_intrin_load_unaligned(const float *values) {
	return kore_float32x4_intrin_load(values);
}

static inline kore_float32x4 kore_float32x4_load(float a, float b, float c, float d) {
	return (kore_float32x4){a, b, c, d};
}

static inline kore_float32x4 kore_float32x4_load_all(float t) {
	return (kore_float32x4){t, t, t, t};
}

static inline void kore_float32x4_store(float *destination, kore_float32x4 value) {
	vst1q_f32(destination, value);
}

static inline void kore_float32x4_store_unaligned(float *destination, kore_float32x4 value) {
	kore_float32x4_store(destination, value);
}

static inline float kore_float32x4_get(kore_float32x4 t, int index) {
	return t[index];
}

static inline kore_float32x4 kore_float32x4_abs(kore_float32x4 t) {
	return vabsq_f32(t);
}

static inline kore_float32x4 kore_float32x4_add(kore_float32x4 a, kore_float32x4 b) {
	return vaddq_f32(a, b);
}

static inline kore_float32x4 kore_float32x4_div(kore_float32x4 a, kore_float32x4 b) {
#if defined(__aarch64__)
	return vdivq_f32(a, b);
#else
	float32x4 inv = vrecpeq_f32(b);
	float32x4 restep = vrecpsq_f32(b, inv);
	inv = vmulq_f32(restep, inv);
	return vmulq_f32(a, inv);
#endif
}

static inline kore_float32x4 kore_float32x4_mul(kore_float32x4 a, kore_float32x4 b) {
	return vmulq_f32(a, b);
}

static inline kore_float32x4 kore_float32x4_neg(kore_float32x4 t) {
	return vnegq_f32(t);
}

static inline kore_float32x4 kore_float32x4_reciprocal_approximation(kore_float32x4 t) {
	return vrecpeq_f32(t);
}

static inline kore_float32x4 kore_float32x4_reciprocal_sqrt_approximation(kore_float32x4 t) {
	return vrsqrteq_f32(t);
}

static inline kore_float32x4 kore_float32x4_sub(kore_float32x4 a, kore_float32x4 b) {
	return vsubq_f32(a, b);
}

static inline kore_float32x4 kore_float32x4_sqrt(kore_float32x4 t) {
#if defined(__aarch64__)
	return vsqrtq_f32(t);
#else
	return vmulq_f32(t, vrsqrteq_f32(t));
#endif
}

static inline kore_float32x4 kore_float32x4_max(kore_float32x4 a, kore_float32x4 b) {
	return vmaxq_f32(a, b);
}

static inline kore_float32x4 kore_float32x4_min(kore_float32x4 a, kore_float32x4 b) {
	return vminq_f32(a, b);
}

static inline kore_float32x4_mask kore_float32x4_cmpeq(kore_float32x4 a, kore_float32x4 b) {
	return vceqq_f32(a, b);
}

static inline kore_float32x4_mask kore_float32x4_cmpge(kore_float32x4 a, kore_float32x4 b) {
	return vcgeq_f32(a, b);
}

static inline kore_float32x4_mask kore_float32x4_cmpgt(kore_float32x4 a, kore_float32x4 b) {
	return vcgtq_f32(a, b);
}

static inline kore_float32x4_mask kore_float32x4_cmple(kore_float32x4 a, kore_float32x4 b) {
	return vcleq_f32(a, b);
}

static inline kore_float32x4_mask kore_float32x4_cmplt(kore_float32x4 a, kore_float32x4 b) {
	return vcltq_f32(a, b);
}

static inline kore_float32x4_mask kore_float32x4_cmpneq(kore_float32x4 a, kore_float32x4 b) {
	return vmvnq_u32(vceqq_f32(a, b));
}

static inline kore_float32x4 kore_float32x4_sel(kore_float32x4 a, kore_float32x4 b, kore_float32x4_mask mask) {
	return vbslq_f32(mask, a, b);
}

static inline kore_float32x4 kore_float32x4_or(kore_float32x4 a, kore_float32x4 b) {
	uint32x4_t acvt = vreinterpretq_u32_f32(a);
	uint32x4_t bcvt = vreinterpretq_u32_f32(b);

	return vreinterpretq_f32_u32(vorrq_u32(acvt, bcvt));
}

static inline kore_float32x4 kore_float32x4_and(kore_float32x4 a, kore_float32x4 b) {
	uint32x4_t acvt = vreinterpretq_u32_f32(a);
	uint32x4_t bcvt = vreinterpretq_u32_f32(b);

	return vreinterpretq_f32_u32(vandq_u32(acvt, bcvt));
}

static inline kore_float32x4 kore_float32x4_xor(kore_float32x4 a, kore_float32x4 b) {
	uint32x4_t acvt = vreinterpretq_u32_f32(a);
	uint32x4_t bcvt = vreinterpretq_u32_f32(b);

	return vreinterpretq_f32_u32(veorq_u32(acvt, bcvt));
}

static inline kore_float32x4 kore_float32x4_not(kore_float32x4 t) {
	uint32x4_t tcvt = vreinterpretq_u32_f32(t);

	return vreinterpretq_f32_u32(vmvnq_u32(tcvt));
}

#define kore_float32x4_shuffle_custom(abcd, efgh, left_1, left_2, right_1, right_2)                                                                            \
	(kore_float32x4) {                                                                                                                                         \
		vgetq_lane_f32((abcd), ((left_1) & 0x3)), vgetq_lane_f32((abcd), ((left_2) & 0x3)), vgetq_lane_f32((efgh), ((right_1) & 0x3)),                         \
		    vgetq_lane_f32((efgh), ((right_2) & 0x3))                                                                                                          \
	}

static inline kore_float32x4 kore_float32x4_shuffle_aebf(kore_float32x4 abcd, kore_float32x4 efgh) {
#if defined(__aarch64__)

	return vzip1q_f32(abcd, efgh);

#else

	float a = vgetq_lane_f32(abcd, 0);
	float b = vgetq_lane_f32(abcd, 1);
	float e = vgetq_lane_f32(efgh, 0);
	float f = vgetq_lane_f32(efgh, 1);

	return (kore_float32x4){a, e, b, f};

#endif
}

static inline kore_float32x4 kore_float32x4_shuffle_cgdh(kore_float32x4 abcd, kore_float32x4 efgh) {
#if defined(__aarch64__)

	return vzip2q_f32(abcd, efgh);

#else

	float c = vgetq_lane_f32(abcd, 2);
	float d = vgetq_lane_f32(abcd, 3);
	float g = vgetq_lane_f32(efgh, 2);
	float h = vgetq_lane_f32(efgh, 3);

	return (kore_float32x4){c, g, d, h};

#endif
}

static inline kore_float32x4 kore_float32x4_shuffle_abef(kore_float32x4 abcd, kore_float32x4 efgh) {
	float32x2_t ab = vget_low_f32(abcd);
	float32x2_t ef = vget_low_f32(efgh);

	return vcombine_f32(ab, ef);
}

static inline kore_float32x4 kore_float32x4_shuffle_ghcd(kore_float32x4 abcd, kore_float32x4 efgh) {
	float32x2_t cd = vget_high_f32(abcd);
	float32x2_t gh = vget_high_f32(efgh);

	return vcombine_f32(gh, cd);
}

#else

#include <math.h>

static inline kore_float32x4 kore_float32x4_intrin_load(const float *values) {
	kore_float32x4 value;
	value.values[0] = values[0];
	value.values[1] = values[1];
	value.values[2] = values[2];
	value.values[3] = values[3];
	return value;
}

static inline kore_float32x4 kore_float32x4_intrin_load_unaligned(const float *values) {
	return kore_float32x4_intrin_load(values);
}

static inline kore_float32x4 kore_float32x4_load(float a, float b, float c, float d) {
	kore_float32x4 value;
	value.values[0] = a;
	value.values[1] = b;
	value.values[2] = c;
	value.values[3] = d;
	return value;
}

static inline kore_float32x4 kore_float32x4_load_all(float t) {
	kore_float32x4 value;
	value.values[0] = t;
	value.values[1] = t;
	value.values[2] = t;
	value.values[3] = t;
	return value;
}

static inline void kore_float32x4_store(float *destination, kore_float32x4 value) {
	destination[0] = value.values[0];
	destination[1] = value.values[1];
	destination[2] = value.values[2];
	destination[3] = value.values[3];
}

static inline void kore_float32x4_store_unaligned(float *destination, kore_float32x4 value) {
	kore_float32x4_store(destination, value);
}

static inline float kore_float32x4_get(kore_float32x4 t, int index) {
	return t.values[index];
}

static inline kore_float32x4 kore_float32x4_abs(kore_float32x4 t) {
	kore_float32x4 value;
	value.values[0] = kore_abs(t.values[0]);
	value.values[1] = kore_abs(t.values[1]);
	value.values[2] = kore_abs(t.values[2]);
	value.values[3] = kore_abs(t.values[3]);
	return value;
}

static inline kore_float32x4 kore_float32x4_add(kore_float32x4 a, kore_float32x4 b) {
	kore_float32x4 value;
	value.values[0] = a.values[0] + b.values[0];
	value.values[1] = a.values[1] + b.values[1];
	value.values[2] = a.values[2] + b.values[2];
	value.values[3] = a.values[3] + b.values[3];
	return value;
}

static inline kore_float32x4 kore_float32x4_div(kore_float32x4 a, kore_float32x4 b) {
	kore_float32x4 value;
	value.values[0] = a.values[0] / b.values[0];
	value.values[1] = a.values[1] / b.values[1];
	value.values[2] = a.values[2] / b.values[2];
	value.values[3] = a.values[3] / b.values[3];
	return value;
}

static inline kore_float32x4 kore_float32x4_mul(kore_float32x4 a, kore_float32x4 b) {
	kore_float32x4 value;
	value.values[0] = a.values[0] * b.values[0];
	value.values[1] = a.values[1] * b.values[1];
	value.values[2] = a.values[2] * b.values[2];
	value.values[3] = a.values[3] * b.values[3];
	return value;
}

static inline kore_float32x4 kore_float32x4_neg(kore_float32x4 t) {
	kore_float32x4 value;
	value.values[0] = -t.values[0];
	value.values[1] = -t.values[1];
	value.values[2] = -t.values[2];
	value.values[3] = -t.values[3];
	return value;
}

static inline kore_float32x4 kore_float32x4_reciprocal_approximation(kore_float32x4 t) {
	kore_float32x4 value;
	value.values[0] = 1.0f / t.values[0];
	value.values[1] = 1.0f / t.values[1];
	value.values[2] = 1.0f / t.values[2];
	value.values[3] = 1.0f / t.values[3];
	return value;
}

static inline kore_float32x4 kore_float32x4_reciprocal_sqrt_approximation(kore_float32x4 t) {
	kore_float32x4 value;
	value.values[0] = 1.0f / sqrtf(t.values[0]);
	value.values[1] = 1.0f / sqrtf(t.values[1]);
	value.values[2] = 1.0f / sqrtf(t.values[2]);
	value.values[3] = 1.0f / sqrtf(t.values[3]);
	return value;
}

static inline kore_float32x4 kore_float32x4_sub(kore_float32x4 a, kore_float32x4 b) {
	kore_float32x4 value;
	value.values[0] = a.values[0] - b.values[0];
	value.values[1] = a.values[1] - b.values[1];
	value.values[2] = a.values[2] - b.values[2];
	value.values[3] = a.values[3] - b.values[3];
	return value;
}

static inline kore_float32x4 kore_float32x4_sqrt(kore_float32x4 t) {
	kore_float32x4 value;
	value.values[0] = sqrtf(t.values[0]);
	value.values[1] = sqrtf(t.values[1]);
	value.values[2] = sqrtf(t.values[2]);
	value.values[3] = sqrtf(t.values[3]);
	return value;
}

static inline kore_float32x4 kore_float32x4_max(kore_float32x4 a, kore_float32x4 b) {
	kore_float32x4 value;
	value.values[0] = kore_max(a.values[0], b.values[0]);
	value.values[1] = kore_max(a.values[1], b.values[1]);
	value.values[2] = kore_max(a.values[2], b.values[2]);
	value.values[3] = kore_max(a.values[3], b.values[3]);
	return value;
}

static inline kore_float32x4 kore_float32x4_min(kore_float32x4 a, kore_float32x4 b) {
	kore_float32x4 value;
	value.values[0] = kore_min(a.values[0], b.values[0]);
	value.values[1] = kore_min(a.values[1], b.values[1]);
	value.values[2] = kore_min(a.values[2], b.values[2]);
	value.values[3] = kore_min(a.values[3], b.values[3]);
	return value;
}

static inline kore_float32x4_mask kore_float32x4_cmpeq(kore_float32x4 a, kore_float32x4 b) {
	uint32_t mask_cvt[4];
	mask_cvt[0] = a.values[0] == b.values[0] ? 0xffffffff : 0;
	mask_cvt[1] = a.values[1] == b.values[1] ? 0xffffffff : 0;
	mask_cvt[2] = a.values[2] == b.values[2] ? 0xffffffff : 0;
	mask_cvt[3] = a.values[3] == b.values[3] ? 0xffffffff : 0;

	kore_float32x4_mask mask;
	memcpy(&mask.values[0], &mask_cvt[0], sizeof(mask_cvt));

	return mask;
}

static inline kore_float32x4_mask kore_float32x4_cmpge(kore_float32x4 a, kore_float32x4 b) {
	uint32_t mask_cvt[4];
	mask_cvt[0] = a.values[0] >= b.values[0] ? 0xffffffff : 0;
	mask_cvt[1] = a.values[1] >= b.values[1] ? 0xffffffff : 0;
	mask_cvt[2] = a.values[2] >= b.values[2] ? 0xffffffff : 0;
	mask_cvt[3] = a.values[3] >= b.values[3] ? 0xffffffff : 0;

	kore_float32x4_mask mask;
	memcpy(&mask.values[0], &mask_cvt[0], sizeof(mask_cvt));

	return mask;
}

static inline kore_float32x4_mask kore_float32x4_cmpgt(kore_float32x4 a, kore_float32x4 b) {
	uint32_t mask_cvt[4];
	mask_cvt[0] = a.values[0] > b.values[0] ? 0xffffffff : 0;
	mask_cvt[1] = a.values[1] > b.values[1] ? 0xffffffff : 0;
	mask_cvt[2] = a.values[2] > b.values[2] ? 0xffffffff : 0;
	mask_cvt[3] = a.values[3] > b.values[3] ? 0xffffffff : 0;

	kore_float32x4_mask mask;
	memcpy(&mask.values[0], &mask_cvt[0], sizeof(mask_cvt));

	return mask;
}

static inline kore_float32x4_mask kore_float32x4_cmple(kore_float32x4 a, kore_float32x4 b) {
	uint32_t mask_cvt[4];
	mask_cvt[0] = a.values[0] <= b.values[0] ? 0xffffffff : 0;
	mask_cvt[1] = a.values[1] <= b.values[1] ? 0xffffffff : 0;
	mask_cvt[2] = a.values[2] <= b.values[2] ? 0xffffffff : 0;
	mask_cvt[3] = a.values[3] <= b.values[3] ? 0xffffffff : 0;

	kore_float32x4_mask mask;
	memcpy(&mask.values[0], &mask_cvt[0], sizeof(mask_cvt));

	return mask;
}

static inline kore_float32x4_mask kore_float32x4_cmplt(kore_float32x4 a, kore_float32x4 b) {
	uint32_t mask_cvt[4];
	mask_cvt[0] = a.values[0] < b.values[0] ? 0xffffffff : 0;
	mask_cvt[1] = a.values[1] < b.values[1] ? 0xffffffff : 0;
	mask_cvt[2] = a.values[2] < b.values[2] ? 0xffffffff : 0;
	mask_cvt[3] = a.values[3] < b.values[3] ? 0xffffffff : 0;

	kore_float32x4_mask mask;
	memcpy(&mask.values[0], &mask_cvt[0], sizeof(mask_cvt));

	return mask;
}

static inline kore_float32x4_mask kore_float32x4_cmpneq(kore_float32x4 a, kore_float32x4 b) {
	uint32_t mask_cvt[4];
	mask_cvt[0] = a.values[0] != b.values[0] ? 0xffffffff : 0;
	mask_cvt[1] = a.values[1] != b.values[1] ? 0xffffffff : 0;
	mask_cvt[2] = a.values[2] != b.values[2] ? 0xffffffff : 0;
	mask_cvt[3] = a.values[3] != b.values[3] ? 0xffffffff : 0;

	kore_float32x4_mask mask;
	memcpy(&mask.values[0], &mask_cvt[0], sizeof(mask_cvt));

	return mask;
}

static inline kore_float32x4 kore_float32x4_sel(kore_float32x4 a, kore_float32x4 b, kore_float32x4_mask mask) {
	kore_float32x4 value;
	value.values[0] = mask.values[0] != 0.0f ? a.values[0] : b.values[0];
	value.values[1] = mask.values[1] != 0.0f ? a.values[1] : b.values[1];
	value.values[2] = mask.values[2] != 0.0f ? a.values[2] : b.values[2];
	value.values[3] = mask.values[3] != 0.0f ? a.values[3] : b.values[3];
	return value;
}

static inline kore_float32x4 kore_float32x4_or(kore_float32x4 a, kore_float32x4 b) {
	uint32_t acvt[4];
	uint32_t bcvt[4];
	memcpy(&acvt[0], &a.values[0], sizeof(a));
	memcpy(&bcvt[0], &b.values[0], sizeof(b));

	acvt[0] |= bcvt[0];
	acvt[1] |= bcvt[1];
	acvt[2] |= bcvt[2];
	acvt[3] |= bcvt[3];

	kore_float32x4 value;
	memcpy(&value.values[0], &acvt[0], sizeof(acvt));

	return value;
}

static inline kore_float32x4 kore_float32x4_and(kore_float32x4 a, kore_float32x4 b) {
	uint32_t acvt[4];
	uint32_t bcvt[4];
	memcpy(&acvt[0], &a.values[0], sizeof(a));
	memcpy(&bcvt[0], &b.values[0], sizeof(b));

	acvt[0] &= bcvt[0];
	acvt[1] &= bcvt[1];
	acvt[2] &= bcvt[2];
	acvt[3] &= bcvt[3];

	kore_float32x4 value;
	memcpy(&value.values[0], &acvt[0], sizeof(acvt));

	return value;
}

static inline kore_float32x4 kore_float32x4_xor(kore_float32x4 a, kore_float32x4 b) {
	uint32_t acvt[4];
	uint32_t bcvt[4];
	memcpy(&acvt[0], &a.values[0], sizeof(a));
	memcpy(&bcvt[0], &b.values[0], sizeof(b));

	acvt[0] ^= bcvt[0];
	acvt[1] ^= bcvt[1];
	acvt[2] ^= bcvt[2];
	acvt[3] ^= bcvt[3];

	kore_float32x4 value;
	memcpy(&value.values[0], &acvt[0], sizeof(acvt));

	return value;
}

static inline kore_float32x4 kore_float32x4_not(kore_float32x4 t) {
	uint32_t tcvt[4];
	memcpy(&tcvt[0], &t.values[0], sizeof(t));

	tcvt[0] = ~tcvt[0];
	tcvt[1] = ~tcvt[1];
	tcvt[2] = ~tcvt[2];
	tcvt[3] = ~tcvt[3];

	kore_float32x4 value;
	memcpy(&value.values[0], &tcvt[0], sizeof(tcvt));

	return value;
}

static inline kore_float32x4 kore_float32x4_shuffle_custom(kore_float32x4 abcd, kore_float32x4 efgh, const uint32_t left_1, const uint32_t left_2,
                                                           const uint32_t right_1, const uint32_t right_2) {
	kore_float32x4 value;

	value.values[0] = abcd.values[left_1 & 0x3];
	value.values[1] = abcd.values[left_2 & 0x3];
	value.values[2] = efgh.values[right_1 & 0x3];
	value.values[3] = efgh.values[right_2 & 0x3];

	return value;
}

static inline kore_float32x4 kore_float32x4_shuffle_aebf(kore_float32x4 abcd, kore_float32x4 efgh) {
	kore_float32x4 value;

	value.values[0] = abcd.values[0];
	value.values[1] = efgh.values[0];
	value.values[2] = abcd.values[1];
	value.values[3] = efgh.values[1];

	return value;
}

static inline kore_float32x4 kore_float32x4_shuffle_cgdh(kore_float32x4 abcd, kore_float32x4 efgh) {
	kore_float32x4 value;

	value.values[0] = abcd.values[2];
	value.values[1] = efgh.values[2];
	value.values[2] = abcd.values[3];
	value.values[3] = efgh.values[3];

	return value;
}

static inline kore_float32x4 kore_float32x4_shuffle_abef(kore_float32x4 abcd, kore_float32x4 efgh) {
	kore_float32x4 value;

	value.values[0] = abcd.values[0];
	value.values[1] = abcd.values[1];
	value.values[2] = efgh.values[0];
	value.values[3] = efgh.values[1];

	return value;
}

static inline kore_float32x4 kore_float32x4_shuffle_ghcd(kore_float32x4 abcd, kore_float32x4 efgh) {
	kore_float32x4 value;

	value.values[0] = efgh.values[2];
	value.values[1] = efgh.values[3];
	value.values[2] = abcd.values[2];
	value.values[3] = abcd.values[3];

	return value;
}

#endif

#ifdef __cplusplus
}
#endif

#endif
