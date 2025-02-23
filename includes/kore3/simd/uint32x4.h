#ifndef KORE_SIMD_UINT32X4_HEADER
#define KORE_SIMD_UINT32X4_HEADER

#include "types.h"

/*! \file uint32x4.h
    \brief Provides 128bit four-element unsigned 32-bit integer SIMD operations which are mapped to equivalent SSE2 or Neon operations.
*/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(KORE_SSE2)

static inline kore_uint32x4 kore_uint32x4_intrin_load(const uint32_t *values) {
	return _mm_load_si128((const kore_uint32x4 *)values);
}

static inline kore_uint32x4 kore_uint32x4_intrin_load_unaligned(const uint32_t *values) {
	return _mm_loadu_si128((const kore_uint32x4 *)values);
}

static inline kore_uint32x4 kore_uint32x4_load(const uint32_t values[4]) {
	return _mm_set_epi32(values[3], values[2], values[1], values[0]);
}

static inline kore_uint32x4 kore_uint32x4_load_all(uint32_t t) {
	return _mm_set1_epi32(t);
}

static inline void kore_uint32x4_store(uint32_t *destination, kore_uint32x4 value) {
	_mm_store_si128((kore_uint32x4 *)destination, value);
}

static inline void kore_uint32x4_store_unaligned(uint32_t *destination, kore_uint32x4 value) {
	_mm_storeu_si128((kore_uint32x4 *)destination, value);
}

static inline uint32_t kore_uint32x4_get(kore_uint32x4 t, int index) {
	union {
		__m128i value;
		uint32_t elements[4];
	} converter;
	converter.value = t;
	return converter.elements[index];
}

static inline kore_uint32x4 kore_uint32x4_add(kore_uint32x4 a, kore_uint32x4 b) {
	return _mm_add_epi32(a, b);
}

static inline kore_uint32x4 kore_uint32x4_sub(kore_uint32x4 a, kore_uint32x4 b) {
	return _mm_sub_epi32(a, b);
}

static inline kore_uint32x4_mask kore_uint32x4_cmpeq(kore_uint32x4 a, kore_uint32x4 b) {
	return _mm_cmpeq_epi32(a, b);
}

static inline kore_uint32x4_mask kore_uint32x4_cmpneq(kore_uint32x4 a, kore_uint32x4 b) {
	return _mm_andnot_si128(_mm_cmpeq_epi32(a, b), _mm_set1_epi32(0xffffffff));
}

static inline kore_uint32x4_mask kore_uint32x4_cmpge(kore_uint32x4 a, kore_uint32x4 b) {
	__m128i bias_by = _mm_set1_epi32((uint32_t)0x80000000);
	return _mm_or_si128(_mm_cmpgt_epi32(_mm_sub_epi32(a, bias_by), _mm_sub_epi32(b, bias_by)), _mm_cmpeq_epi32(a, b));
}

static inline kore_uint32x4_mask kore_uint32x4_cmpgt(kore_uint32x4 a, kore_uint32x4 b) {
	__m128i bias_by = _mm_set1_epi32((uint32_t)0x80000000);
	return _mm_cmpgt_epi32(_mm_sub_epi32(a, bias_by), _mm_sub_epi32(b, bias_by));
}

static inline kore_uint32x4_mask kore_uint32x4_cmple(kore_uint32x4 a, kore_uint32x4 b) {
	__m128i bias_by = _mm_set1_epi32((uint32_t)0x80000000);
	return _mm_or_si128(_mm_cmplt_epi32(_mm_sub_epi32(a, bias_by), _mm_sub_epi32(b, bias_by)), _mm_cmpeq_epi32(a, b));
}

static inline kore_uint32x4_mask kore_uint32x4_cmplt(kore_uint32x4 a, kore_uint32x4 b) {
	__m128i bias_by = _mm_set1_epi32((uint32_t)0x80000000);
	return _mm_cmplt_epi32(_mm_sub_epi32(a, bias_by), _mm_sub_epi32(b, bias_by));
}

static inline kore_uint32x4 kore_uint32x4_sel(kore_uint32x4 a, kore_uint32x4 b, kore_uint32x4_mask mask) {
	return _mm_xor_si128(b, _mm_and_si128(mask, _mm_xor_si128(a, b)));
}

static inline kore_uint32x4 kore_uint32x4_max(kore_uint32x4 a, kore_uint32x4 b) {
	return kore_uint32x4_sel(a, b, kore_uint32x4_cmpgt(a, b));
}

static inline kore_uint32x4 kore_uint32x4_min(kore_uint32x4 a, kore_uint32x4 b) {
	return kore_uint32x4_sel(a, b, kore_uint32x4_cmplt(a, b));
}

static inline kore_uint32x4 kore_uint32x4_or(kore_uint32x4 a, kore_uint32x4 b) {
	return _mm_or_si128(a, b);
}

static inline kore_uint32x4 kore_uint32x4_and(kore_uint32x4 a, kore_uint32x4 b) {
	return _mm_and_si128(a, b);
}

static inline kore_uint32x4 kore_uint32x4_xor(kore_uint32x4 a, kore_uint32x4 b) {
	return _mm_xor_si128(a, b);
}

static inline kore_uint32x4 kore_uint32x4_not(kore_uint32x4 t) {
	return _mm_xor_si128(t, _mm_set1_epi32(0xffffffff));
}

#define kore_uint32x4_shift_left(t, shift) _mm_slli_epi32((t), (shift))

#define kore_uint32x4_shift_right(t, shift) _mm_srli_epi32((t), (shift))

#elif defined(KORE_NEON)

static inline kore_uint32x4 kore_uint32x4_intrin_load(const uint32_t *values) {
	return vld1q_u32(values);
}

static inline kore_uint32x4 kore_uint32x4_intrin_load_unaligned(const uint32_t *values) {
	return kore_uint32x4_intrin_load(values);
}

static inline kore_uint32x4 kore_uint32x4_load(const uint32_t values[4]) {
	return (kore_uint32x4){values[0], values[1], values[2], values[3]};
}

static inline kore_uint32x4 kore_uint32x4_load_all(uint32_t t) {
	return (kore_uint32x4){t, t, t, t};
}

static inline void kore_uint32x4_store(uint32_t *destination, kore_uint32x4 value) {
	vst1q_u32(destination, value);
}

static inline void kore_uint32x4_store_unaligned(uint32_t *destination, kore_uint32x4 value) {
	kore_uint32x4_store(destination, value);
}

static inline uint32_t kore_uint32x4_get(kore_uint32x4 t, int index) {
	return t[index];
}

static inline kore_uint32x4 kore_uint32x4_add(kore_uint32x4 a, kore_uint32x4 b) {
	return vaddq_u32(a, b);
}

static inline kore_uint32x4 kore_uint32x4_sub(kore_uint32x4 a, kore_uint32x4 b) {
	return vsubq_u32(a, b);
}

static inline kore_uint32x4 kore_uint32x4_max(kore_uint32x4 a, kore_uint32x4 b) {
	return vmaxq_u32(a, b);
}

static inline kore_uint32x4 kore_uint32x4_min(kore_uint32x4 a, kore_uint32x4 b) {
	return vminq_u32(a, b);
}

static inline kore_uint32x4_mask kore_uint32x4_cmpeq(kore_uint32x4 a, kore_uint32x4 b) {
	return vceqq_u32(a, b);
}

static inline kore_uint32x4_mask kore_uint32x4_cmpneq(kore_uint32x4 a, kore_uint32x4 b) {
	return vmvnq_u32(vceqq_u32(a, b));
}

static inline kore_uint32x4_mask kore_uint32x4_cmpge(kore_uint32x4 a, kore_uint32x4 b) {
	return vcgeq_u32(a, b);
}

static inline kore_uint32x4_mask kore_uint32x4_cmpgt(kore_uint32x4 a, kore_uint32x4 b) {
	return vcgtq_u32(a, b);
}

static inline kore_uint32x4_mask kore_uint32x4_cmple(kore_uint32x4 a, kore_uint32x4 b) {
	return vcleq_u32(a, b);
}

static inline kore_uint32x4_mask kore_uint32x4_cmplt(kore_uint32x4 a, kore_uint32x4 b) {
	return vcltq_u32(a, b);
}

static inline kore_uint32x4 kore_uint32x4_sel(kore_uint32x4 a, kore_uint32x4 b, kore_uint32x4_mask mask) {
	return vbslq_u32(mask, a, b);
}

static inline kore_uint32x4 kore_uint32x4_or(kore_uint32x4 a, kore_uint32x4 b) {
	return vorrq_u32(a, b);
}

static inline kore_uint32x4 kore_uint32x4_and(kore_uint32x4 a, kore_uint32x4 b) {
	return vandq_u32(a, b);
}

static inline kore_uint32x4 kore_uint32x4_xor(kore_uint32x4 a, kore_uint32x4 b) {
	return veorq_u32(a, b);
}

static inline kore_uint32x4 kore_uint32x4_not(kore_uint32x4 t) {
	return vmvnq_u32(t);
}

#define kore_uint32x4_shift_left(t, shift) vshlq_n_u32((t), (shift))

#define kore_uint32x4_shift_right(t, shift) vshrq_n_u32((t), (shift))

#else

static inline kore_uint32x4 kore_uint32x4_intrin_load(const uint32_t *values) {
	kore_uint32x4 value;
	value.values[0] = values[0];
	value.values[1] = values[1];
	value.values[2] = values[2];
	value.values[3] = values[3];
	return value;
}

static inline kore_uint32x4 kore_uint32x4_intrin_load_unaligned(const uint32_t *values) {
	return kore_uint32x4_intrin_load(values);
}

static inline kore_uint32x4 kore_uint32x4_load(const uint32_t values[4]) {
	kore_uint32x4 value;
	value.values[0] = values[0];
	value.values[1] = values[1];
	value.values[2] = values[2];
	value.values[3] = values[3];
	return value;
}

static inline kore_uint32x4 kore_uint32x4_load_all(uint32_t t) {
	kore_uint32x4 value;
	value.values[0] = t;
	value.values[1] = t;
	value.values[2] = t;
	value.values[3] = t;
	return value;
}

static inline void kore_uint32x4_store(uint32_t *destination, kore_uint32x4 value) {
	destination[0] = value.values[0];
	destination[1] = value.values[1];
	destination[2] = value.values[2];
	destination[3] = value.values[3];
}

static inline void kore_uint32x4_store_unaligned(uint32_t *destination, kore_uint32x4 value) {
	kore_uint32x4_store(destination, value);
}

static inline uint32_t kore_uint32x4_get(kore_uint32x4 t, int index) {
	return t.values[index];
}

static inline kore_uint32x4 kore_uint32x4_add(kore_uint32x4 a, kore_uint32x4 b) {
	kore_uint32x4 value;
	value.values[0] = a.values[0] + b.values[0];
	value.values[1] = a.values[1] + b.values[1];
	value.values[2] = a.values[2] + b.values[2];
	value.values[3] = a.values[3] + b.values[3];
	return value;
}

static inline kore_uint32x4 kore_uint32x4_sub(kore_uint32x4 a, kore_uint32x4 b) {
	kore_uint32x4 value;
	value.values[0] = a.values[0] - b.values[0];
	value.values[1] = a.values[1] - b.values[1];
	value.values[2] = a.values[2] - b.values[2];
	value.values[3] = a.values[3] - b.values[3];
	return value;
}

static inline kore_uint32x4 kore_uint32x4_max(kore_uint32x4 a, kore_uint32x4 b) {
	kore_uint32x4 value;
	value.values[0] = a.values[0] > b.values[0] ? a.values[0] : b.values[0];
	value.values[1] = a.values[1] > b.values[1] ? a.values[1] : b.values[1];
	value.values[2] = a.values[2] > b.values[2] ? a.values[2] : b.values[2];
	value.values[3] = a.values[3] > b.values[3] ? a.values[3] : b.values[3];
	return value;
}

static inline kore_uint32x4 kore_uint32x4_min(kore_uint32x4 a, kore_uint32x4 b) {
	kore_uint32x4 value;
	value.values[0] = a.values[0] > b.values[0] ? b.values[0] : a.values[0];
	value.values[1] = a.values[1] > b.values[1] ? b.values[1] : a.values[1];
	value.values[2] = a.values[2] > b.values[2] ? b.values[2] : a.values[2];
	value.values[3] = a.values[3] > b.values[3] ? b.values[3] : a.values[3];
	return value;
}

static inline kore_uint32x4_mask kore_uint32x4_cmpeq(kore_uint32x4 a, kore_uint32x4 b) {
	kore_uint32x4_mask mask;
	mask.values[0] = a.values[0] == b.values[0] ? 0xffffffff : 0;
	mask.values[1] = a.values[1] == b.values[1] ? 0xffffffff : 0;
	mask.values[2] = a.values[2] == b.values[2] ? 0xffffffff : 0;
	mask.values[3] = a.values[3] == b.values[3] ? 0xffffffff : 0;
	return mask;
}

static inline kore_uint32x4_mask kore_uint32x4_cmpneq(kore_uint32x4 a, kore_uint32x4 b) {
	kore_uint32x4_mask mask;
	mask.values[0] = a.values[0] != b.values[0] ? 0xffffffff : 0;
	mask.values[1] = a.values[1] != b.values[1] ? 0xffffffff : 0;
	mask.values[2] = a.values[2] != b.values[2] ? 0xffffffff : 0;
	mask.values[3] = a.values[3] != b.values[3] ? 0xffffffff : 0;
	return mask;
}

static inline kore_uint32x4_mask kore_uint32x4_cmpge(kore_uint32x4 a, kore_uint32x4 b) {
	kore_uint32x4_mask mask;
	mask.values[0] = a.values[0] >= b.values[0] ? 0xffffffff : 0;
	mask.values[1] = a.values[1] >= b.values[1] ? 0xffffffff : 0;
	mask.values[2] = a.values[2] >= b.values[2] ? 0xffffffff : 0;
	mask.values[3] = a.values[3] >= b.values[3] ? 0xffffffff : 0;
	return mask;
}

static inline kore_uint32x4_mask kore_uint32x4_cmpgt(kore_uint32x4 a, kore_uint32x4 b) {
	kore_uint32x4_mask mask;
	mask.values[0] = a.values[0] > b.values[0] ? 0xffffffff : 0;
	mask.values[1] = a.values[1] > b.values[1] ? 0xffffffff : 0;
	mask.values[2] = a.values[2] > b.values[2] ? 0xffffffff : 0;
	mask.values[3] = a.values[3] > b.values[3] ? 0xffffffff : 0;
	return mask;
}

static inline kore_uint32x4_mask kore_uint32x4_cmple(kore_uint32x4 a, kore_uint32x4 b) {
	kore_uint32x4_mask mask;
	mask.values[0] = a.values[0] <= b.values[0] ? 0xffffffff : 0;
	mask.values[1] = a.values[1] <= b.values[1] ? 0xffffffff : 0;
	mask.values[2] = a.values[2] <= b.values[2] ? 0xffffffff : 0;
	mask.values[3] = a.values[3] <= b.values[3] ? 0xffffffff : 0;
	return mask;
}

static inline kore_uint32x4_mask kore_uint32x4_cmplt(kore_uint32x4 a, kore_uint32x4 b) {
	kore_uint32x4_mask mask;
	mask.values[0] = a.values[0] < b.values[0] ? 0xffffffff : 0;
	mask.values[1] = a.values[1] < b.values[1] ? 0xffffffff : 0;
	mask.values[2] = a.values[2] < b.values[2] ? 0xffffffff : 0;
	mask.values[3] = a.values[3] < b.values[3] ? 0xffffffff : 0;
	return mask;
}

static inline kore_uint32x4 kore_uint32x4_sel(kore_uint32x4 a, kore_uint32x4 b, kore_uint32x4_mask mask) {
	kore_uint32x4 value;
	value.values[0] = mask.values[0] != 0 ? a.values[0] : b.values[0];
	value.values[1] = mask.values[1] != 0 ? a.values[1] : b.values[1];
	value.values[2] = mask.values[2] != 0 ? a.values[2] : b.values[2];
	value.values[3] = mask.values[3] != 0 ? a.values[3] : b.values[3];
	return value;
}

static inline kore_uint32x4 kore_uint32x4_or(kore_uint32x4 a, kore_uint32x4 b) {
	kore_uint32x4 value;
	value.values[0] = a.values[0] | b.values[0];
	value.values[1] = a.values[1] | b.values[1];
	value.values[2] = a.values[2] | b.values[2];
	value.values[3] = a.values[3] | b.values[3];
	return value;
}

static inline kore_uint32x4 kore_uint32x4_and(kore_uint32x4 a, kore_uint32x4 b) {
	kore_uint32x4 value;
	value.values[0] = a.values[0] & b.values[0];
	value.values[1] = a.values[1] & b.values[1];
	value.values[2] = a.values[2] & b.values[2];
	value.values[3] = a.values[3] & b.values[3];
	return value;
}

static inline kore_uint32x4 kore_uint32x4_xor(kore_uint32x4 a, kore_uint32x4 b) {
	kore_uint32x4 value;
	value.values[0] = a.values[0] ^ b.values[0];
	value.values[1] = a.values[1] ^ b.values[1];
	value.values[2] = a.values[2] ^ b.values[2];
	value.values[3] = a.values[3] ^ b.values[3];
	return value;
}

static inline kore_uint32x4 kore_uint32x4_not(kore_uint32x4 t) {
	kore_uint32x4 value;
	value.values[0] = ~t.values[0];
	value.values[1] = ~t.values[1];
	value.values[2] = ~t.values[2];
	value.values[3] = ~t.values[3];
	return value;
}

static inline kore_uint32x4 kore_uint32x4_shift_left(kore_uint32x4 t, const int shift) {
	kore_uint32x4 value;
	value.values[0] = t.values[0] << shift;
	value.values[1] = t.values[1] << shift;
	value.values[2] = t.values[2] << shift;
	value.values[3] = t.values[3] << shift;

	return value;
}

static inline kore_uint32x4 kore_uint32x4_shift_right(kore_uint32x4 t, const int shift) {
	kore_uint32x4 value;
	value.values[0] = t.values[0] >> shift;
	value.values[1] = t.values[1] >> shift;
	value.values[2] = t.values[2] >> shift;
	value.values[3] = t.values[3] >> shift;

	return value;
}

#endif

#ifdef __cplusplus
}
#endif

#endif
