#ifndef KORE_SIMD_INT16X8_HEADER
#define KORE_SIMD_INT16X8_HEADER

#include "types.h"

/*! \file int16x8.h
    \brief Provides 128bit eight-element signed 16-bit integer SIMD operations which are mapped to equivalent SSE2 or Neon operations.
*/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(KORE_SSE2)

static inline kore_int16x8 kore_int16x8_intrin_load(const int16_t *values) {
	return _mm_load_si128((const kore_int16x8 *)values);
}

static inline kore_int16x8 kore_int16x8_intrin_load_unaligned(const int16_t *values) {
	return _mm_loadu_si128((const kore_int16x8 *)values);
}

static inline kore_int16x8 kore_int16x8_load(const int16_t values[8]) {
	return _mm_set_epi16(values[7], values[6], values[5], values[4], values[3], values[2], values[1], values[0]);
}

static inline kore_int16x8 kore_int16x8_load_all(int16_t t) {
	return _mm_set1_epi16(t);
}

static inline void kore_int16x8_store(int16_t *destination, kore_int16x8 value) {
	_mm_store_si128((kore_int16x8 *)destination, value);
}

static inline void kore_int16x8_store_unaligned(int16_t *destination, kore_int16x8 value) {
	_mm_storeu_si128((kore_int16x8 *)destination, value);
}

static inline int16_t kore_int16x8_get(kore_int16x8 t, int index) {
	union {
		__m128i value;
		int16_t elements[8];
	} converter;
	converter.value = t;
	return converter.elements[index];
}

static inline kore_int16x8 kore_int16x8_add(kore_int16x8 a, kore_int16x8 b) {
	return _mm_add_epi16(a, b);
}

static inline kore_int16x8 kore_int16x8_sub(kore_int16x8 a, kore_int16x8 b) {
	return _mm_sub_epi16(a, b);
}

static inline kore_int16x8 kore_int16x8_max(kore_int16x8 a, kore_int16x8 b) {
	__m128i mask = _mm_cmpgt_epi16(a, b);
	return _mm_xor_si128(b, _mm_and_si128(mask, _mm_xor_si128(a, b)));
}

static inline kore_int16x8 kore_int16x8_min(kore_int16x8 a, kore_int16x8 b) {
	__m128i mask = _mm_cmplt_epi16(a, b);
	return _mm_xor_si128(b, _mm_and_si128(mask, _mm_xor_si128(a, b)));
}

static inline kore_int16x8_mask kore_int16x8_cmpeq(kore_int16x8 a, kore_int16x8 b) {
	return _mm_cmpeq_epi16(a, b);
}

static inline kore_int16x8_mask kore_int16x8_cmpge(kore_int16x8 a, kore_int16x8 b) {
	return _mm_or_si128(_mm_cmpgt_epi16(a, b), _mm_cmpeq_epi16(a, b));
}

static inline kore_int16x8_mask kore_int16x8_cmpgt(kore_int16x8 a, kore_int16x8 b) {
	return _mm_cmpgt_epi16(a, b);
}

static inline kore_int16x8_mask kore_int16x8_cmple(kore_int16x8 a, kore_int16x8 b) {
	return _mm_or_si128(_mm_cmplt_epi16(a, b), _mm_cmpeq_epi16(a, b));
}

static inline kore_int16x8_mask kore_int16x8_cmplt(kore_int16x8 a, kore_int16x8 b) {
	return _mm_cmplt_epi16(a, b);
}

static inline kore_int16x8_mask kore_int16x8_cmpneq(kore_int16x8 a, kore_int16x8 b) {
	return _mm_andnot_si128(_mm_cmpeq_epi16(a, b), _mm_set1_epi32(0xffffffff));
}

static inline kore_int16x8 kore_int16x8_sel(kore_int16x8 a, kore_int16x8 b, kore_int16x8_mask mask) {
	return _mm_xor_si128(b, _mm_and_si128(mask, _mm_xor_si128(a, b)));
}

static inline kore_int16x8 kore_int16x8_or(kore_int16x8 a, kore_int16x8 b) {
	return _mm_or_si128(a, b);
}

static inline kore_int16x8 kore_int16x8_and(kore_int16x8 a, kore_int16x8 b) {
	return _mm_and_si128(a, b);
}

static inline kore_int16x8 kore_int16x8_xor(kore_int16x8 a, kore_int16x8 b) {
	return _mm_xor_si128(a, b);
}

static inline kore_int16x8 kore_int16x8_not(kore_int16x8 t) {
	return _mm_xor_si128(t, _mm_set1_epi32(0xffffffff));
}

#elif defined(KORE_NEON)

static inline kore_int16x8 kore_int16x8_intrin_load(const int16_t *values) {
	return vld1q_s16(values);
}

static inline kore_int16x8 kore_int16x8_intrin_load_unaligned(const int16_t *values) {
	return kore_int16x8_intrin_load(values);
}

static inline kore_int16x8 kore_int16x8_load(const int16_t values[8]) {
	return (kore_int16x8){values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7]};
}

static inline kore_int16x8 kore_int16x8_load_all(int16_t t) {
	return (kore_int16x8){t, t, t, t, t, t, t, t};
}

static inline void kore_int16x8_store(int16_t *destination, kore_int16x8 value) {
	vst1q_s16(destination, value);
}

static inline void kore_int16x8_store_unaligned(int16_t *destination, kore_int16x8 value) {
	kore_int16x8_store(destination, value);
}

static inline int16_t kore_int16x8_get(kore_int16x8 t, int index) {
	return t[index];
}

static inline kore_int16x8 kore_int16x8_add(kore_int16x8 a, kore_int16x8 b) {
	return vaddq_s16(a, b);
}

static inline kore_int16x8 kore_int16x8_sub(kore_int16x8 a, kore_int16x8 b) {
	return vsubq_s16(a, b);
}

static inline kore_int16x8 kore_int16x8_max(kore_int16x8 a, kore_int16x8 b) {
	return vmaxq_s16(a, b);
}

static inline kore_int16x8 kore_int16x8_min(kore_int16x8 a, kore_int16x8 b) {
	return vminq_s16(a, b);
}

static inline kore_int16x8_mask kore_int16x8_cmpeq(kore_int16x8 a, kore_int16x8 b) {
	return vceqq_s16(a, b);
}

static inline kore_int16x8_mask kore_int16x8_cmpge(kore_int16x8 a, kore_int16x8 b) {
	return vcgeq_s16(a, b);
}

static inline kore_int16x8_mask kore_int16x8_cmpgt(kore_int16x8 a, kore_int16x8 b) {
	return vcgtq_s16(a, b);
}

static inline kore_int16x8_mask kore_int16x8_cmple(kore_int16x8 a, kore_int16x8 b) {
	return vcleq_s16(a, b);
}

static inline kore_int16x8_mask kore_int16x8_cmplt(kore_int16x8 a, kore_int16x8 b) {
	return vcltq_s16(a, b);
}

static inline kore_int16x8_mask kore_int16x8_cmpneq(kore_int16x8 a, kore_int16x8 b) {
	return vmvnq_u16(vceqq_s16(a, b));
}

static inline kore_int16x8 kore_int16x8_sel(kore_int16x8 a, kore_int16x8 b, kore_int16x8_mask mask) {
	return vbslq_s16(mask, a, b);
}

static inline kore_int16x8 kore_int16x8_or(kore_int16x8 a, kore_int16x8 b) {
	return vorrq_s16(a, b);
}

static inline kore_int16x8 kore_int16x8_and(kore_int16x8 a, kore_int16x8 b) {
	return vandq_s16(a, b);
}

static inline kore_int16x8 kore_int16x8_xor(kore_int16x8 a, kore_int16x8 b) {
	return veorq_s16(a, b);
}

static inline kore_int16x8 kore_int16x8_not(kore_int16x8 t) {
	return vmvnq_s16(t);
}

#else

static inline kore_int16x8 kore_int16x8_intrin_load(const int16_t *values) {
	kore_int16x8 value;
	value.values[0] = values[0];
	value.values[1] = values[1];
	value.values[2] = values[2];
	value.values[3] = values[3];
	value.values[4] = values[4];
	value.values[5] = values[5];
	value.values[6] = values[6];
	value.values[7] = values[7];
	return value;
}

static inline kore_int16x8 kore_int16x8_intrin_load_unaligned(const int16_t *values) {
	kore_int16x8_intrin_load(values);
}

static inline kore_int16x8 kore_int16x8_load(const int16_t values[8]) {
	kore_int16x8 value;
	value.values[0] = values[0];
	value.values[1] = values[1];
	value.values[2] = values[2];
	value.values[3] = values[3];
	value.values[4] = values[4];
	value.values[5] = values[5];
	value.values[6] = values[6];
	value.values[7] = values[7];
	return value;
}

static inline kore_int16x8 kore_int16x8_load_all(int16_t t) {
	kore_int16x8 value;
	value.values[0] = t;
	value.values[1] = t;
	value.values[2] = t;
	value.values[3] = t;
	value.values[4] = t;
	value.values[5] = t;
	value.values[6] = t;
	value.values[7] = t;
	return value;
}

static inline void kore_int16x8_store(int16_t *destination, kore_int16x8 value) {
	destination[0] = value.values[0];
	destination[1] = value.values[1];
	destination[2] = value.values[2];
	destination[3] = value.values[3];
	destination[4] = value.values[4];
	destination[5] = value.values[5];
	destination[6] = value.values[6];
	destination[7] = value.values[7];
}

static inline void kore_int16x8_store_unaligned(int16_t *destination, kore_int16x8 value) {
	kore_int16x8_store(destination, value);
}

static inline int16_t kore_int16x8_get(kore_int16x8 t, int index) {
	return t.values[index];
}

static inline kore_int16x8 kore_int16x8_add(kore_int16x8 a, kore_int16x8 b) {
	kore_int16x8 value;
	value.values[0] = a.values[0] + b.values[0];
	value.values[1] = a.values[1] + b.values[1];
	value.values[2] = a.values[2] + b.values[2];
	value.values[3] = a.values[3] + b.values[3];
	value.values[4] = a.values[4] + b.values[4];
	value.values[5] = a.values[5] + b.values[5];
	value.values[6] = a.values[6] + b.values[6];
	value.values[7] = a.values[7] + b.values[7];
	return value;
}

static inline kore_int16x8 kore_int16x8_sub(kore_int16x8 a, kore_int16x8 b) {
	kore_int16x8 value;
	value.values[0] = a.values[0] - b.values[0];
	value.values[1] = a.values[1] - b.values[1];
	value.values[2] = a.values[2] - b.values[2];
	value.values[3] = a.values[3] - b.values[3];
	value.values[4] = a.values[4] - b.values[4];
	value.values[5] = a.values[5] - b.values[5];
	value.values[6] = a.values[6] - b.values[6];
	value.values[7] = a.values[7] - b.values[7];
	return value;
}

static inline kore_int16x8 kore_int16x8_max(kore_int16x8 a, kore_int16x8 b) {
	kore_int16x8 value;
	value.values[0] = a.values[0] > b.values[0] ? a.values[0] : b.values[0];
	value.values[1] = a.values[1] > b.values[1] ? a.values[1] : b.values[1];
	value.values[2] = a.values[2] > b.values[2] ? a.values[2] : b.values[2];
	value.values[3] = a.values[3] > b.values[3] ? a.values[3] : b.values[3];
	value.values[4] = a.values[4] > b.values[4] ? a.values[4] : b.values[4];
	value.values[5] = a.values[5] > b.values[5] ? a.values[5] : b.values[5];
	value.values[6] = a.values[6] > b.values[6] ? a.values[6] : b.values[6];
	value.values[7] = a.values[7] > b.values[7] ? a.values[7] : b.values[7];
	return value;
}

static inline kore_int16x8 kore_int16x8_min(kore_int16x8 a, kore_int16x8 b) {
	kore_int16x8 value;
	value.values[0] = a.values[0] > b.values[0] ? b.values[0] : a.values[0];
	value.values[1] = a.values[1] > b.values[1] ? b.values[1] : a.values[1];
	value.values[2] = a.values[2] > b.values[2] ? b.values[2] : a.values[2];
	value.values[3] = a.values[3] > b.values[3] ? b.values[3] : a.values[3];
	value.values[4] = a.values[4] > b.values[4] ? b.values[4] : a.values[4];
	value.values[5] = a.values[5] > b.values[5] ? b.values[5] : a.values[5];
	value.values[6] = a.values[6] > b.values[6] ? b.values[6] : a.values[6];
	value.values[7] = a.values[7] > b.values[7] ? b.values[7] : a.values[7];
	return value;
}

static inline kore_int16x8_mask kore_int16x8_cmpeq(kore_int16x8 a, kore_int16x8 b) {
	kore_int16x8_mask mask;
	mask.values[0] = a.values[0] == b.values[0] ? 0xffff : 0;
	mask.values[1] = a.values[1] == b.values[1] ? 0xffff : 0;
	mask.values[2] = a.values[2] == b.values[2] ? 0xffff : 0;
	mask.values[3] = a.values[3] == b.values[3] ? 0xffff : 0;
	mask.values[4] = a.values[4] == b.values[4] ? 0xffff : 0;
	mask.values[5] = a.values[5] == b.values[5] ? 0xffff : 0;
	mask.values[6] = a.values[6] == b.values[6] ? 0xffff : 0;
	mask.values[7] = a.values[7] == b.values[7] ? 0xffff : 0;
	return mask;
}

static inline kore_int16x8_mask kore_int16x8_cmpge(kore_int16x8 a, kore_int16x8 b) {
	kore_int16x8_mask mask;
	mask.values[0] = a.values[0] >= b.values[0] ? 0xffff : 0;
	mask.values[1] = a.values[1] >= b.values[1] ? 0xffff : 0;
	mask.values[2] = a.values[2] >= b.values[2] ? 0xffff : 0;
	mask.values[3] = a.values[3] >= b.values[3] ? 0xffff : 0;
	mask.values[4] = a.values[4] >= b.values[4] ? 0xffff : 0;
	mask.values[5] = a.values[5] >= b.values[5] ? 0xffff : 0;
	mask.values[6] = a.values[6] >= b.values[6] ? 0xffff : 0;
	mask.values[7] = a.values[7] >= b.values[7] ? 0xffff : 0;
	return mask;
}

static inline kore_int16x8_mask kore_int16x8_cmpgt(kore_int16x8 a, kore_int16x8 b) {
	kore_int16x8_mask mask;
	mask.values[0] = a.values[0] > b.values[0] ? 0xffff : 0;
	mask.values[1] = a.values[1] > b.values[1] ? 0xffff : 0;
	mask.values[2] = a.values[2] > b.values[2] ? 0xffff : 0;
	mask.values[3] = a.values[3] > b.values[3] ? 0xffff : 0;
	mask.values[4] = a.values[4] > b.values[4] ? 0xffff : 0;
	mask.values[5] = a.values[5] > b.values[5] ? 0xffff : 0;
	mask.values[6] = a.values[6] > b.values[6] ? 0xffff : 0;
	mask.values[7] = a.values[7] > b.values[7] ? 0xffff : 0;
	return mask;
}

static inline kore_int16x8_mask kore_int16x8_cmple(kore_int16x8 a, kore_int16x8 b) {
	kore_int16x8_mask mask;
	mask.values[0] = a.values[0] <= b.values[0] ? 0xffff : 0;
	mask.values[1] = a.values[1] <= b.values[1] ? 0xffff : 0;
	mask.values[2] = a.values[2] <= b.values[2] ? 0xffff : 0;
	mask.values[3] = a.values[3] <= b.values[3] ? 0xffff : 0;
	mask.values[4] = a.values[4] <= b.values[4] ? 0xffff : 0;
	mask.values[5] = a.values[5] <= b.values[5] ? 0xffff : 0;
	mask.values[6] = a.values[6] <= b.values[6] ? 0xffff : 0;
	mask.values[7] = a.values[7] <= b.values[7] ? 0xffff : 0;
	return mask;
}

static inline kore_int16x8_mask kore_int16x8_cmplt(kore_int16x8 a, kore_int16x8 b) {
	kore_int16x8_mask mask;
	mask.values[0] = a.values[0] < b.values[0] ? 0xffff : 0;
	mask.values[1] = a.values[1] < b.values[1] ? 0xffff : 0;
	mask.values[2] = a.values[2] < b.values[2] ? 0xffff : 0;
	mask.values[3] = a.values[3] < b.values[3] ? 0xffff : 0;
	mask.values[4] = a.values[4] < b.values[4] ? 0xffff : 0;
	mask.values[5] = a.values[5] < b.values[5] ? 0xffff : 0;
	mask.values[6] = a.values[6] < b.values[6] ? 0xffff : 0;
	mask.values[7] = a.values[7] < b.values[7] ? 0xffff : 0;
	return mask;
}

static inline kore_int16x8_mask kore_int16x8_cmpneq(kore_int16x8 a, kore_int16x8 b) {
	kore_int16x8_mask mask;
	mask.values[0] = a.values[0] != b.values[0] ? 0xffff : 0;
	mask.values[1] = a.values[1] != b.values[1] ? 0xffff : 0;
	mask.values[2] = a.values[2] != b.values[2] ? 0xffff : 0;
	mask.values[3] = a.values[3] != b.values[3] ? 0xffff : 0;
	mask.values[4] = a.values[4] != b.values[4] ? 0xffff : 0;
	mask.values[5] = a.values[5] != b.values[5] ? 0xffff : 0;
	mask.values[6] = a.values[6] != b.values[6] ? 0xffff : 0;
	mask.values[7] = a.values[7] != b.values[7] ? 0xffff : 0;
	return mask;
}

static inline kore_int16x8 kore_int16x8_sel(kore_int16x8 a, kore_int16x8 b, kore_int16x8_mask mask) {
	kore_int16x8 value;
	value.values[0] = mask.values[0] != 0 ? a.values[0] : b.values[0];
	value.values[1] = mask.values[1] != 0 ? a.values[1] : b.values[1];
	value.values[2] = mask.values[2] != 0 ? a.values[2] : b.values[2];
	value.values[3] = mask.values[3] != 0 ? a.values[3] : b.values[3];
	value.values[4] = mask.values[4] != 0 ? a.values[4] : b.values[4];
	value.values[5] = mask.values[5] != 0 ? a.values[5] : b.values[5];
	value.values[6] = mask.values[6] != 0 ? a.values[6] : b.values[6];
	value.values[7] = mask.values[7] != 0 ? a.values[7] : b.values[7];
	return value;
}

static inline kore_int16x8 kore_int16x8_or(kore_int16x8 a, kore_int16x8 b) {
	kore_int16x8 value;
	value.values[0] = a.values[0] | b.values[0];
	value.values[1] = a.values[1] | b.values[1];
	value.values[2] = a.values[2] | b.values[2];
	value.values[3] = a.values[3] | b.values[3];
	value.values[4] = a.values[4] | b.values[4];
	value.values[5] = a.values[5] | b.values[5];
	value.values[6] = a.values[6] | b.values[6];
	value.values[7] = a.values[7] | b.values[7];
	return value;
}

static inline kore_int16x8 kore_int16x8_and(kore_int16x8 a, kore_int16x8 b) {
	kore_int16x8 value;
	value.values[0] = a.values[0] & b.values[0];
	value.values[1] = a.values[1] & b.values[1];
	value.values[2] = a.values[2] & b.values[2];
	value.values[3] = a.values[3] & b.values[3];
	value.values[4] = a.values[4] & b.values[4];
	value.values[5] = a.values[5] & b.values[5];
	value.values[6] = a.values[6] & b.values[6];
	value.values[7] = a.values[7] & b.values[7];
	return value;
}

static inline kore_int16x8 kore_int16x8_xor(kore_int16x8 a, kore_int16x8 b) {
	kore_int16x8 value;
	value.values[0] = a.values[0] ^ b.values[0];
	value.values[1] = a.values[1] ^ b.values[1];
	value.values[2] = a.values[2] ^ b.values[2];
	value.values[3] = a.values[3] ^ b.values[3];
	value.values[4] = a.values[4] ^ b.values[4];
	value.values[5] = a.values[5] ^ b.values[5];
	value.values[6] = a.values[6] ^ b.values[6];
	value.values[7] = a.values[7] ^ b.values[7];
	return value;
}

static inline kore_int16x8 kore_int16x8_not(kore_int16x8 t) {
	kore_int16x8 value;
	value.values[0] = ~t.values[0];
	value.values[1] = ~t.values[1];
	value.values[2] = ~t.values[2];
	value.values[3] = ~t.values[3];
	value.values[4] = ~t.values[4];
	value.values[5] = ~t.values[5];
	value.values[6] = ~t.values[6];
	value.values[7] = ~t.values[7];
	return value;
}

#endif

#ifdef __cplusplus
}
#endif

#endif
