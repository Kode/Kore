#ifndef KORE_SIMD_UINT16X8_HEADER
#define KORE_SIMD_UINT16X8_HEADER

#include "types.h"

/*! \file uint16x8.h
    \brief Provides 128bit eight-element unsigned 16-bit integer SIMD operations which are mapped to equivalent SSE2 or Neon operations.
*/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(KORE_SSE2)

static inline kore_uint16x8 kore_uint16x8_intrin_load(const uint16_t *values) {
	return _mm_load_si128((const kore_uint16x8 *)values);
}

static inline kore_uint16x8 kore_uint16x8_intrin_load_unaligned(const uint16_t *values) {
	return _mm_loadu_si128((const kore_uint16x8 *)values);
}

static inline kore_uint16x8 kore_uint16x8_load(const uint16_t values[8]) {
	return _mm_set_epi16(values[7], values[6], values[5], values[4], values[3], values[2], values[1], values[0]);
}

static inline kore_uint16x8 kore_uint16x8_load_all(uint16_t t) {
	return _mm_set1_epi16(t);
}

static inline void kore_uint16x8_store(uint16_t *destination, kore_uint16x8 value) {
	_mm_store_si128((kore_uint16x8 *)destination, value);
}

static inline void kore_uint16x8_store_unaligned(uint16_t *destination, kore_uint16x8 value) {
	_mm_storeu_si128((kore_uint16x8 *)destination, value);
}

static inline uint16_t kore_uint16x8_get(kore_uint16x8 t, int index) {
	union {
		__m128i  value;
		uint16_t elements[8];
	} converter;
	converter.value = t;
	return converter.elements[index];
}

static inline kore_uint16x8 kore_uint16x8_add(kore_uint16x8 a, kore_uint16x8 b) {
	return _mm_add_epi16(a, b);
}

static inline kore_uint16x8 kore_uint16x8_sub(kore_uint16x8 a, kore_uint16x8 b) {
	return _mm_sub_epi16(a, b);
}

static inline kore_uint16x8_mask kore_uint16x8_cmpeq(kore_uint16x8 a, kore_uint16x8 b) {
	return _mm_cmpeq_epi16(a, b);
}

static inline kore_uint16x8_mask kore_uint16x8_cmpneq(kore_uint16x8 a, kore_uint16x8 b) {
	return _mm_andnot_si128(_mm_cmpeq_epi16(a, b), _mm_set1_epi32(0xffffffff));
}

static inline kore_uint16x8_mask kore_uint16x8_cmpge(kore_uint16x8 a, kore_uint16x8 b) {
	__m128i bias_by = _mm_set1_epi16((uint16_t)0x8000);
	return _mm_or_si128(_mm_cmpgt_epi16(_mm_sub_epi16(a, bias_by), _mm_sub_epi16(b, bias_by)), _mm_cmpeq_epi16(a, b));
}

static inline kore_uint16x8_mask kore_uint16x8_cmpgt(kore_uint16x8 a, kore_uint16x8 b) {
	__m128i bias_by = _mm_set1_epi16((uint16_t)0x8000);
	return _mm_cmpgt_epi16(_mm_sub_epi16(a, bias_by), _mm_sub_epi16(b, bias_by));
}

static inline kore_uint16x8_mask kore_uint16x8_cmple(kore_uint16x8 a, kore_uint16x8 b) {
	__m128i bias_by = _mm_set1_epi16((uint16_t)0x8000);
	return _mm_or_si128(_mm_cmplt_epi16(_mm_sub_epi16(a, bias_by), _mm_sub_epi16(b, bias_by)), _mm_cmpeq_epi16(a, b));
}

static inline kore_uint16x8_mask kore_uint16x8_cmplt(kore_uint16x8 a, kore_uint16x8 b) {
	__m128i bias_by = _mm_set1_epi16((uint16_t)0x8000);
	return _mm_cmplt_epi16(_mm_sub_epi16(a, bias_by), _mm_sub_epi16(b, bias_by));
}

static inline kore_uint16x8 kore_uint16x8_sel(kore_uint16x8 a, kore_uint16x8 b, kore_uint16x8_mask mask) {
	return _mm_xor_si128(b, _mm_and_si128(mask, _mm_xor_si128(a, b)));
}

static inline kore_uint16x8 kore_uint16x8_max(kore_uint16x8 a, kore_uint16x8 b) {
	return kore_uint16x8_sel(a, b, kore_uint16x8_cmpgt(a, b));
}

static inline kore_uint16x8 kore_uint16x8_min(kore_uint16x8 a, kore_uint16x8 b) {
	return kore_uint16x8_sel(a, b, kore_uint16x8_cmplt(a, b));
}

static inline kore_uint16x8 kore_uint16x8_or(kore_uint16x8 a, kore_uint16x8 b) {
	return _mm_or_si128(a, b);
}

static inline kore_uint16x8 kore_uint16x8_and(kore_uint16x8 a, kore_uint16x8 b) {
	return _mm_and_si128(a, b);
}

static inline kore_uint16x8 kore_uint16x8_xor(kore_uint16x8 a, kore_uint16x8 b) {
	return _mm_xor_si128(a, b);
}

static inline kore_uint16x8 kore_uint16x8_not(kore_uint16x8 t) {
	return _mm_xor_si128(t, _mm_set1_epi32(0xffffffff));
}

#define kore_uint16x8_shift_left(t, shift) _mm_slli_epi16((t), (shift))

#define kore_uint16x8_shift_right(t, shift) _mm_srli_epi16((t), (shift))

#elif defined(KORE_NEON)

static inline kore_uint16x8 kore_uint16x8_intrin_load(const uint16_t *values) {
	return vld1q_u16(values);
}

static inline kore_uint16x8 kore_uint16x8_intrin_load_unaligned(const uint16_t *values) {
	return kore_uint16x8_intrin_load(values);
}

static inline kore_uint16x8 kore_uint16x8_load(const uint16_t values[8]) {
	return (kore_uint16x8){values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7]};
}

static inline kore_uint16x8 kore_uint16x8_load_all(uint16_t t) {
	return (kore_uint16x8){t, t, t, t, t, t, t, t};
}

static inline void kore_uint16x8_store(uint16_t *destination, kore_uint16x8 value) {
	vst1q_u16(destination, value);
}

static inline void kore_uint16x8_store_unaligned(uint16_t *destination, kore_uint16x8 value) {
	kore_uint16x8_store(destination, value);
}

static inline uint16_t kore_uint16x8_get(kore_uint16x8 t, int index) {
	return t[index];
}

static inline kore_uint16x8 kore_uint16x8_add(kore_uint16x8 a, kore_uint16x8 b) {
	return vaddq_u16(a, b);
}

static inline kore_uint16x8 kore_uint16x8_sub(kore_uint16x8 a, kore_uint16x8 b) {
	return vsubq_u16(a, b);
}

static inline kore_uint16x8 kore_uint16x8_max(kore_uint16x8 a, kore_uint16x8 b) {
	return vmaxq_u16(a, b);
}

static inline kore_uint16x8 kore_uint16x8_min(kore_uint16x8 a, kore_uint16x8 b) {
	return vminq_u16(a, b);
}

static inline kore_uint16x8_mask kore_uint16x8_cmpeq(kore_uint16x8 a, kore_uint16x8 b) {
	return vceqq_u16(a, b);
}

static inline kore_uint16x8_mask kore_uint16x8_cmpneq(kore_uint16x8 a, kore_uint16x8 b) {
	return vmvnq_u16(vceqq_u16(a, b));
}

static inline kore_uint16x8_mask kore_uint16x8_cmpge(kore_uint16x8 a, kore_uint16x8 b) {
	return vcgeq_u16(a, b);
}

static inline kore_uint16x8_mask kore_uint16x8_cmpgt(kore_uint16x8 a, kore_uint16x8 b) {
	return vcgtq_u16(a, b);
}

static inline kore_uint16x8_mask kore_uint16x8_cmple(kore_uint16x8 a, kore_uint16x8 b) {
	return vcleq_u16(a, b);
}

static inline kore_uint16x8_mask kore_uint16x8_cmplt(kore_uint16x8 a, kore_uint16x8 b) {
	return vcltq_u16(a, b);
}

static inline kore_uint16x8 kore_uint16x8_sel(kore_uint16x8 a, kore_uint16x8 b, kore_uint16x8_mask mask) {
	return vbslq_u16(mask, a, b);
}

static inline kore_uint16x8 kore_uint16x8_or(kore_uint16x8 a, kore_uint16x8 b) {
	return vorrq_u16(a, b);
}

static inline kore_uint16x8 kore_uint16x8_and(kore_uint16x8 a, kore_uint16x8 b) {
	return vandq_u16(a, b);
}

static inline kore_uint16x8 kore_uint16x8_xor(kore_uint16x8 a, kore_uint16x8 b) {
	return veorq_u16(a, b);
}

static inline kore_uint16x8 kore_uint16x8_not(kore_uint16x8 t) {
	return vmvnq_u16(t);
}

#define kore_uint16x8_shift_left(t, shift) vshlq_n_u16((t), (shift))

#define kore_uint16x8_shift_right(t, shift) vshrq_n_u16((t), (shift))

#else

static inline kore_uint16x8 kore_uint16x8_intrin_load(const uint16_t *values) {
	kore_uint16x8 value;
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

static inline kore_uint16x8 kore_uint16x8_intrin_load_unaligned(const uint16_t *values) {
	return kore_uint16x8_intrin_load(values);
}

static inline kore_uint16x8 kore_uint16x8_load(const uint16_t values[8]) {
	kore_uint16x8 value;
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

static inline kore_uint16x8 kore_uint16x8_load_all(uint16_t t) {
	kore_uint16x8 value;
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

static inline void kore_uint16x8_store(uint16_t *destination, kore_uint16x8 value) {
	destination[0] = value.values[0];
	destination[1] = value.values[1];
	destination[2] = value.values[2];
	destination[3] = value.values[3];
	destination[4] = value.values[4];
	destination[5] = value.values[5];
	destination[6] = value.values[6];
	destination[7] = value.values[7];
}

static inline void kore_uint16x8_store_unaligned(uint16_t *destination, kore_uint16x8 value) {
	kore_uint16x8_store(destination, value);
}

static inline uint16_t kore_uint16x8_get(kore_uint16x8 t, int index) {
	return t.values[index];
}

static inline kore_uint16x8 kore_uint16x8_add(kore_uint16x8 a, kore_uint16x8 b) {
	kore_uint16x8 value;
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

static inline kore_uint16x8 kore_uint16x8_sub(kore_uint16x8 a, kore_uint16x8 b) {
	kore_uint16x8 value;
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

static inline kore_uint16x8 kore_uint16x8_max(kore_uint16x8 a, kore_uint16x8 b) {
	kore_uint16x8 value;
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

static inline kore_uint16x8 kore_uint16x8_min(kore_uint16x8 a, kore_uint16x8 b) {
	kore_uint16x8 value;
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

static inline kore_uint16x8_mask kore_uint16x8_cmpeq(kore_uint16x8 a, kore_uint16x8 b) {
	kore_uint16x8_mask mask;
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

static inline kore_uint16x8_mask kore_uint16x8_cmpneq(kore_uint16x8 a, kore_uint16x8 b) {
	kore_uint16x8_mask mask;
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

static inline kore_uint16x8_mask kore_uint16x8_cmpge(kore_uint16x8 a, kore_uint16x8 b) {
	kore_uint16x8_mask mask;
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

static inline kore_uint16x8_mask kore_uint16x8_cmpgt(kore_uint16x8 a, kore_uint16x8 b) {
	kore_uint16x8_mask mask;
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

static inline kore_uint16x8_mask kore_uint16x8_cmple(kore_uint16x8 a, kore_uint16x8 b) {
	kore_uint16x8_mask mask;
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

static inline kore_uint16x8_mask kore_uint16x8_cmplt(kore_uint16x8 a, kore_uint16x8 b) {
	kore_uint16x8_mask mask;
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

static inline kore_uint16x8 kore_uint16x8_sel(kore_uint16x8 a, kore_uint16x8 b, kore_uint16x8_mask mask) {
	kore_uint16x8 value;
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

static inline kore_uint16x8 kore_uint16x8_or(kore_uint16x8 a, kore_uint16x8 b) {
	kore_uint16x8 value;
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

static inline kore_uint16x8 kore_uint16x8_and(kore_uint16x8 a, kore_uint16x8 b) {
	kore_uint16x8 value;
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

static inline kore_uint16x8 kore_uint16x8_xor(kore_uint16x8 a, kore_uint16x8 b) {
	kore_uint16x8 value;
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

static inline kore_uint16x8 kore_uint16x8_not(kore_uint16x8 t) {
	kore_uint16x8 value;
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

static inline kore_uint16x8 kore_uint16x8_shift_left(kore_uint16x8 t, const int shift) {
	kore_uint16x8 value;
	value.values[0] = t.values[0] << shift;
	value.values[1] = t.values[1] << shift;
	value.values[2] = t.values[2] << shift;
	value.values[3] = t.values[3] << shift;
	value.values[4] = t.values[4] << shift;
	value.values[5] = t.values[5] << shift;
	value.values[6] = t.values[6] << shift;
	value.values[7] = t.values[7] << shift;

	return value;
}

static inline kore_uint16x8 kore_uint16x8_shift_right(kore_uint16x8 t, const int shift) {
	kore_uint16x8 value;
	value.values[0] = t.values[0] >> shift;
	value.values[1] = t.values[1] >> shift;
	value.values[2] = t.values[2] >> shift;
	value.values[3] = t.values[3] >> shift;
	value.values[4] = t.values[4] >> shift;
	value.values[5] = t.values[5] >> shift;
	value.values[6] = t.values[6] >> shift;
	value.values[7] = t.values[7] >> shift;

	return value;
}

#endif

#ifdef __cplusplus
}
#endif

#endif
