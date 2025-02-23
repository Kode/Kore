#ifndef KORE_SIMD_INT32X4_HEADER
#define KORE_SIMD_INT32X4_HEADER

#include "types.h"

/*! \file int32x4.h
    \brief Provides 128bit four-element signed 32-bit integer SIMD operations which are mapped to equivalent SSE2 or Neon operations.
*/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(KORE_SSE2)

static inline kore_int32x4 kore_int32x4_intrin_load(const int32_t *values) {
	return _mm_load_si128((const kore_int32x4 *)values);
}

static inline kore_int32x4 kore_int32x4_intrin_load_unaligned(const int32_t *values) {
	return _mm_loadu_si128((const kore_int32x4 *)values);
}

static inline kore_int32x4 kore_int32x4_load(const int32_t values[4]) {
	return _mm_set_epi32(values[3], values[2], values[1], values[0]);
}

static inline kore_int32x4 kore_int32x4_load_all(int32_t t) {
	return _mm_set1_epi32(t);
}

static inline void kore_int32x4_store(int32_t *destination, kore_int32x4 value) {
	_mm_store_si128((kore_int32x4 *)destination, value);
}

static inline void kore_int32x4_store_unaligned(int32_t *destination, kore_int32x4 value) {
	_mm_storeu_si128((kore_int32x4 *)destination, value);
}

static inline int32_t kore_int32x4_get(kore_int32x4 t, int index) {
	union {
		__m128i value;
		int32_t elements[4];
	} converter;
	converter.value = t;
	return converter.elements[index];
}

static inline kore_int32x4 kore_int32x4_add(kore_int32x4 a, kore_int32x4 b) {
	return _mm_add_epi32(a, b);
}

static inline kore_int32x4 kore_int32x4_sub(kore_int32x4 a, kore_int32x4 b) {
	return _mm_sub_epi32(a, b);
}

static inline kore_int32x4 kore_int32x4_max(kore_int32x4 a, kore_int32x4 b) {
	__m128i mask = _mm_cmpgt_epi32(a, b);
	return _mm_xor_si128(b, _mm_and_si128(mask, _mm_xor_si128(a, b)));
}

static inline kore_int32x4 kore_int32x4_min(kore_int32x4 a, kore_int32x4 b) {
	__m128i mask = _mm_cmplt_epi32(a, b);
	return _mm_xor_si128(b, _mm_and_si128(mask, _mm_xor_si128(a, b)));
}

static inline kore_int32x4_mask kore_int32x4_cmpeq(kore_int32x4 a, kore_int32x4 b) {
	return _mm_cmpeq_epi32(a, b);
}

static inline kore_int32x4_mask kore_int32x4_cmpge(kore_int32x4 a, kore_int32x4 b) {
	return _mm_or_si128(_mm_cmpgt_epi32(a, b), _mm_cmpeq_epi32(a, b));
}

static inline kore_int32x4_mask kore_int32x4_cmpgt(kore_int32x4 a, kore_int32x4 b) {
	return _mm_cmpgt_epi32(a, b);
}

static inline kore_int32x4_mask kore_int32x4_cmple(kore_int32x4 a, kore_int32x4 b) {
	return _mm_or_si128(_mm_cmplt_epi32(a, b), _mm_cmpeq_epi32(a, b));
}

static inline kore_int32x4_mask kore_int32x4_cmplt(kore_int32x4 a, kore_int32x4 b) {
	return _mm_cmplt_epi32(a, b);
}

static inline kore_int32x4_mask kore_int32x4_cmpneq(kore_int32x4 a, kore_int32x4 b) {
	return _mm_andnot_si128(_mm_cmpeq_epi32(a, b), _mm_set1_epi32(0xffffffff));
}

static inline kore_int32x4 kore_int32x4_sel(kore_int32x4 a, kore_int32x4 b, kore_int32x4_mask mask) {
	return _mm_xor_si128(b, _mm_and_si128(mask, _mm_xor_si128(a, b)));
}

static inline kore_int32x4 kore_int32x4_or(kore_int32x4 a, kore_int32x4 b) {
	return _mm_or_si128(a, b);
}

static inline kore_int32x4 kore_int32x4_and(kore_int32x4 a, kore_int32x4 b) {
	return _mm_and_si128(a, b);
}

static inline kore_int32x4 kore_int32x4_xor(kore_int32x4 a, kore_int32x4 b) {
	return _mm_xor_si128(a, b);
}

static inline kore_int32x4 kore_int32x4_not(kore_int32x4 t) {
	return _mm_xor_si128(t, _mm_set1_epi32(0xffffffff));
}

#elif defined(KORE_NEON)

static inline kore_int32x4 kore_int32x4_intrin_load(const int32 *values) {
	return vld1q_s32(values);
}

static inline kore_int32x4 kore_int32x4_intrin_load_unaligned(const int32 *values) {
	return kore_int32x4_intrin_load(values);
}

static inline kore_int32x4 kore_int32x4_load(const int32 values[4]) {
	return (kore_int32x4){values[0], values[1], values[2], values[3]};
}

static inline kore_int32x4 kore_int32x4_load_all(int32 t) {
	return (kore_int32x4){t, t, t, t};
}

static inline void kore_int32x4_store(int32 *destination, kore_int32x4 value) {
	vst1q_s32(destination, value);
}

static inline void kore_int32x4_store_unaligned(int32 *destination, kore_int32x4 value) {
	kore_int32x4_store(destination, value);
}

static inline int32 kore_int32x4_get(kore_int32x4 t, int index) {
	return t[index];
}

static inline kore_int32x4 kore_int32x4_add(kore_int32x4 a, kore_int32x4 b) {
	return vaddq_s32(a, b);
}

static inline kore_int32x4 kore_int32x4_sub(kore_int32x4 a, kore_int32x4 b) {
	return vsubq_s32(a, b);
}

static inline kore_int32x4 kore_int32x4_max(kore_int32x4 a, kore_int32x4 b) {
	return vmaxq_s32(a, b);
}

static inline kore_int32x4 kore_int32x4_min(kore_int32x4 a, kore_int32x4 b) {
	return vminq_s32(a, b);
}

static inline kore_int32x4_mask kore_int32x4_cmpeq(kore_int32x4 a, kore_int32x4 b) {
	return vceqq_s32(a, b);
}

static inline kore_int32x4_mask kore_int32x4_cmpge(kore_int32x4 a, kore_int32x4 b) {
	return vcgeq_s32(a, b);
}

static inline kore_int32x4_mask kore_int32x4_cmpgt(kore_int32x4 a, kore_int32x4 b) {
	return vcgtq_s32(a, b);
}

static inline kore_int32x4_mask kore_int32x4_cmple(kore_int32x4 a, kore_int32x4 b) {
	return vcleq_s32(a, b);
}

static inline kore_int32x4_mask kore_int32x4_cmplt(kore_int32x4 a, kore_int32x4 b) {
	return vcltq_s32(a, b);
}

static inline kore_int32x4_mask kore_int32x4_cmpneq(kore_int32x4 a, kore_int32x4 b) {
	return vmvnq_u32(vceqq_s32(a, b));
}

static inline kore_int32x4 kore_int32x4_sel(kore_int32x4 a, kore_int32x4 b, kore_int32x4_mask mask) {
	return vbslq_s32(mask, a, b);
}

static inline kore_int32x4 kore_int32x4_or(kore_int32x4 a, kore_int32x4 b) {
	return vorrq_s32(a, b);
}

static inline kore_int32x4 kore_int32x4_and(kore_int32x4 a, kore_int32x4 b) {
	return vandq_s32(a, b);
}

static inline kore_int32x4 kore_int32x4_xor(kore_int32x4 a, kore_int32x4 b) {
	return veorq_s32(a, b);
}

static inline kore_int32x4 kore_int32x4_not(kore_int32x4 t) {
	return vmvnq_s32(t);
}

#else

static inline kore_int32x4 kore_int32x4_intrin_load(const int32_t *values) {
	kore_int32x4 value;
	value.values[0] = values[0];
	value.values[1] = values[1];
	value.values[2] = values[2];
	value.values[3] = values[3];
	return value;
}

static inline kore_int32x4 kore_int32x4_intrin_load_unaligned(const int32_t *values) {
	return kore_int32x4_intrin_load(values);
}

static inline kore_int32x4 kore_int32x4_load(const int32_t values[4]) {
	kore_int32x4 value;
	value.values[0] = values[0];
	value.values[1] = values[1];
	value.values[2] = values[2];
	value.values[3] = values[3];
	return value;
}

static inline kore_int32x4 kore_int32x4_load_all(int32_t t) {
	kore_int32x4 value;
	value.values[0] = t;
	value.values[1] = t;
	value.values[2] = t;
	value.values[3] = t;
	return value;
}

static inline void kore_int32x4_store(int32_t *destination, kore_int32x4 value) {
	destination[0] = value.values[0];
	destination[1] = value.values[1];
	destination[2] = value.values[2];
	destination[3] = value.values[3];
}

static inline void kore_int32x4_store_unaligned(int32_t *destination, kore_int32x4 value) {
	kore_int32x4_store(destination, value);
}

static inline int32_t kore_int32x4_get(kore_int32x4 t, int index) {
	return t.values[index];
}

static inline kore_int32x4 kore_int32x4_add(kore_int32x4 a, kore_int32x4 b) {
	kore_int32x4 value;
	value.values[0] = a.values[0] + b.values[0];
	value.values[1] = a.values[1] + b.values[1];
	value.values[2] = a.values[2] + b.values[2];
	value.values[3] = a.values[3] + b.values[3];
	return value;
}

static inline kore_int32x4 kore_int32x4_sub(kore_int32x4 a, kore_int32x4 b) {
	kore_int32x4 value;
	value.values[0] = a.values[0] - b.values[0];
	value.values[1] = a.values[1] - b.values[1];
	value.values[2] = a.values[2] - b.values[2];
	value.values[3] = a.values[3] - b.values[3];
	return value;
}

static inline kore_int32x4 kore_int32x4_max(kore_int32x4 a, kore_int32x4 b) {
	kore_int32x4 value;
	value.values[0] = a.values[0] > b.values[0] ? a.values[0] : b.values[0];
	value.values[1] = a.values[1] > b.values[1] ? a.values[1] : b.values[1];
	value.values[2] = a.values[2] > b.values[2] ? a.values[2] : b.values[2];
	value.values[3] = a.values[3] > b.values[3] ? a.values[3] : b.values[3];
	return value;
}

static inline kore_int32x4 kore_int32x4_min(kore_int32x4 a, kore_int32x4 b) {
	kore_int32x4 value;
	value.values[0] = a.values[0] > b.values[0] ? b.values[0] : a.values[0];
	value.values[1] = a.values[1] > b.values[1] ? b.values[1] : a.values[1];
	value.values[2] = a.values[2] > b.values[2] ? b.values[2] : a.values[2];
	value.values[3] = a.values[3] > b.values[3] ? b.values[3] : a.values[3];
	return value;
}

static inline kore_int32x4_mask kore_int32x4_cmpeq(kore_int32x4 a, kore_int32x4 b) {
	kore_int32x4_mask mask;
	mask.values[0] = a.values[0] == b.values[0] ? 0xffffffff : 0;
	mask.values[1] = a.values[1] == b.values[1] ? 0xffffffff : 0;
	mask.values[2] = a.values[2] == b.values[2] ? 0xffffffff : 0;
	mask.values[3] = a.values[3] == b.values[3] ? 0xffffffff : 0;
	return mask;
}

static inline kore_int32x4_mask kore_int32x4_cmpge(kore_int32x4 a, kore_int32x4 b) {
	kore_int32x4_mask mask;
	mask.values[0] = a.values[0] >= b.values[0] ? 0xffffffff : 0;
	mask.values[1] = a.values[1] >= b.values[1] ? 0xffffffff : 0;
	mask.values[2] = a.values[2] >= b.values[2] ? 0xffffffff : 0;
	mask.values[3] = a.values[3] >= b.values[3] ? 0xffffffff : 0;
	return mask;
}

static inline kore_int32x4_mask kore_int32x4_cmpgt(kore_int32x4 a, kore_int32x4 b) {
	kore_int32x4_mask mask;
	mask.values[0] = a.values[0] > b.values[0] ? 0xffffffff : 0;
	mask.values[1] = a.values[1] > b.values[1] ? 0xffffffff : 0;
	mask.values[2] = a.values[2] > b.values[2] ? 0xffffffff : 0;
	mask.values[3] = a.values[3] > b.values[3] ? 0xffffffff : 0;
	return mask;
}

static inline kore_int32x4_mask kore_int32x4_cmple(kore_int32x4 a, kore_int32x4 b) {
	kore_int32x4_mask mask;
	mask.values[0] = a.values[0] <= b.values[0] ? 0xffffffff : 0;
	mask.values[1] = a.values[1] <= b.values[1] ? 0xffffffff : 0;
	mask.values[2] = a.values[2] <= b.values[2] ? 0xffffffff : 0;
	mask.values[3] = a.values[3] <= b.values[3] ? 0xffffffff : 0;
	return mask;
}

static inline kore_int32x4_mask kore_int32x4_cmplt(kore_int32x4 a, kore_int32x4 b) {
	kore_int32x4_mask mask;
	mask.values[0] = a.values[0] < b.values[0] ? 0xffffffff : 0;
	mask.values[1] = a.values[1] < b.values[1] ? 0xffffffff : 0;
	mask.values[2] = a.values[2] < b.values[2] ? 0xffffffff : 0;
	mask.values[3] = a.values[3] < b.values[3] ? 0xffffffff : 0;
	return mask;
}

static inline kore_int32x4_mask kore_int32x4_cmpneq(kore_int32x4 a, kore_int32x4 b) {
	kore_int32x4_mask mask;
	mask.values[0] = a.values[0] != b.values[0] ? 0xffffffff : 0;
	mask.values[1] = a.values[1] != b.values[1] ? 0xffffffff : 0;
	mask.values[2] = a.values[2] != b.values[2] ? 0xffffffff : 0;
	mask.values[3] = a.values[3] != b.values[3] ? 0xffffffff : 0;
	return mask;
}

static inline kore_int32x4 kore_int32x4_sel(kore_int32x4 a, kore_int32x4 b, kore_int32x4_mask mask) {
	kore_int32x4 value;
	value.values[0] = mask.values[0] != 0 ? a.values[0] : b.values[0];
	value.values[1] = mask.values[1] != 0 ? a.values[1] : b.values[1];
	value.values[2] = mask.values[2] != 0 ? a.values[2] : b.values[2];
	value.values[3] = mask.values[3] != 0 ? a.values[3] : b.values[3];
	return value;
}

static inline kore_int32x4 kore_int32x4_or(kore_int32x4 a, kore_int32x4 b) {
	kore_int32x4 value;
	value.values[0] = a.values[0] | b.values[0];
	value.values[1] = a.values[1] | b.values[1];
	value.values[2] = a.values[2] | b.values[2];
	value.values[3] = a.values[3] | b.values[3];
	return value;
}

static inline kore_int32x4 kore_int32x4_and(kore_int32x4 a, kore_int32x4 b) {
	kore_int32x4 value;
	value.values[0] = a.values[0] & b.values[0];
	value.values[1] = a.values[1] & b.values[1];
	value.values[2] = a.values[2] & b.values[2];
	value.values[3] = a.values[3] & b.values[3];
	return value;
}

static inline kore_int32x4 kore_int32x4_xor(kore_int32x4 a, kore_int32x4 b) {
	kore_int32x4 value;
	value.values[0] = a.values[0] ^ b.values[0];
	value.values[1] = a.values[1] ^ b.values[1];
	value.values[2] = a.values[2] ^ b.values[2];
	value.values[3] = a.values[3] ^ b.values[3];
	return value;
}

static inline kore_int32x4 kore_int32x4_not(kore_int32x4 t) {
	kore_int32x4 value;
	value.values[0] = ~t.values[0];
	value.values[1] = ~t.values[1];
	value.values[2] = ~t.values[2];
	value.values[3] = ~t.values[3];
	return value;
}

#endif

#ifdef __cplusplus
}
#endif

#endif
