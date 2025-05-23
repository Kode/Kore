#include <kore3/log.h>
#include <kore3/math/core.h>
#include <kore3/simd/float32x4.h>
#include <kore3/simd/int16x8.h>
#include <kore3/simd/int32x4.h>
#include <kore3/simd/int8x16.h>
#include <kore3/simd/uint16x8.h>
#include <kore3/simd/uint32x4.h>
#include <kore3/simd/uint8x16.h>
#include <kore3/system.h>

#include <math.h>
#include <stdbool.h>

#define EPSILON 0.00001f

static int totalests = 0;

static bool check_f32(const char *name, kore_float32x4 result, const float expected[4], float epsilon) {
	++totalests;
	bool success = true;
	for (int i = 0; i < 4; ++i) {
		if (kore_abs(kore_float32x4_get(result, i) - expected[i]) > epsilon) {
			success = false;
		}
	}
	kore_log(KORE_LOG_LEVEL_ERROR, "Test %s %s", name, success ? "PASS" : "FAIL");
	if (!success) {
		kore_log(KORE_LOG_LEVEL_INFO, "\texpected {%f, %f, %f, %f} got {%f, %f, %f, %f}", expected[0], expected[1], expected[2], expected[3],
		         kore_float32x4_get(result, 0), kore_float32x4_get(result, 1), kore_float32x4_get(result, 2), kore_float32x4_get(result, 3));
	}
	return success;
}

static bool check_i8(const char *name, kore_int8x16 result, const int8_t expected[16]) {
	++totalests;
	bool success = true;
	for (int i = 0; i < 16; ++i) {
		if (kore_int8x16_get(result, i) != expected[i]) {
			success = false;
		}
	}
	kore_log(KORE_LOG_LEVEL_ERROR, "Test %s %s", name, success ? "PASS" : "FAIL");
	if (!success) {
		kore_log(
		    KORE_LOG_LEVEL_INFO,
		    "\texpected {%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d} got {%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d}",
		    expected[0], expected[1], expected[2], expected[3], expected[4], expected[5], expected[6], expected[7], expected[8], expected[9], expected[10],
		    expected[11], expected[12], expected[13], expected[14], expected[15], kore_int8x16_get(result, 0), kore_int8x16_get(result, 1),
		    kore_int8x16_get(result, 2), kore_int8x16_get(result, 3), kore_int8x16_get(result, 4), kore_int8x16_get(result, 5), kore_int8x16_get(result, 6),
		    kore_int8x16_get(result, 7), kore_int8x16_get(result, 8), kore_int8x16_get(result, 9), kore_int8x16_get(result, 10), kore_int8x16_get(result, 11),
		    kore_int8x16_get(result, 12), kore_int8x16_get(result, 13), kore_int8x16_get(result, 14), kore_int8x16_get(result, 15));
	}
	return success;
}

static bool check_u8(const char *name, kore_uint8x16 result, const uint8_t expected[16]) {
	++totalests;
	bool success = true;
	for (int i = 0; i < 16; ++i) {
		if ((kore_uint8x16_get(result, i) ^ expected[i]) != 0) {
			success = false;
		}
	}
	kore_log(KORE_LOG_LEVEL_ERROR, "Test %s %s", name, success ? "PASS" : "FAIL");
	if (!success) {
		kore_log(
		    KORE_LOG_LEVEL_INFO,
		    "\texpected {%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u} got {%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u}",
		    expected[0], expected[1], expected[2], expected[3], expected[4], expected[5], expected[6], expected[7], expected[8], expected[9], expected[10],
		    expected[11], expected[12], expected[13], expected[14], expected[15], kore_uint8x16_get(result, 0), kore_uint8x16_get(result, 1),
		    kore_uint8x16_get(result, 2), kore_uint8x16_get(result, 3), kore_uint8x16_get(result, 4), kore_uint8x16_get(result, 5),
		    kore_uint8x16_get(result, 6), kore_uint8x16_get(result, 7), kore_uint8x16_get(result, 8), kore_uint8x16_get(result, 9),
		    kore_uint8x16_get(result, 10), kore_uint8x16_get(result, 11), kore_uint8x16_get(result, 12), kore_uint8x16_get(result, 13),
		    kore_uint8x16_get(result, 14), kore_uint8x16_get(result, 15));
	}
	return success;
}

static bool check_i16(const char *name, kore_int16x8 result, const int16_t expected[8]) {
	++totalests;
	bool success = true;
	for (int i = 0; i < 8; ++i) {
		if (kore_int16x8_get(result, i) != expected[i]) {
			success = false;
		}
	}
	kore_log(KORE_LOG_LEVEL_ERROR, "Test %s %s", name, success ? "PASS" : "FAIL");
	if (!success) {
		kore_log(KORE_LOG_LEVEL_INFO, "\texpected {%d, %d, %d, %d, %d, %d, %d, %d} got {%d, %d, %d, %d, %d, %d, %d, %d}", expected[0], expected[1], expected[2],
		         expected[3], expected[4], expected[5], expected[6], expected[7], kore_int16x8_get(result, 0), kore_int16x8_get(result, 1),
		         kore_int16x8_get(result, 2), kore_int16x8_get(result, 3), kore_int16x8_get(result, 4), kore_int16x8_get(result, 5),
		         kore_int16x8_get(result, 6), kore_int16x8_get(result, 7));
	}
	return success;
}

static bool check_u16(const char *name, kore_uint16x8 result, const uint16_t expected[8]) {
	++totalests;
	bool success = true;
	for (int i = 0; i < 8; ++i) {
		if ((kore_uint16x8_get(result, i) ^ expected[i]) != 0) {
			success = false;
		}
	}
	kore_log(KORE_LOG_LEVEL_ERROR, "Test %s %s", name, success ? "PASS" : "FAIL");
	if (!success) {
		kore_log(KORE_LOG_LEVEL_INFO, "\texpected {%lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu} got {%lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu}", expected[0],
		         expected[1], expected[2], expected[3], expected[4], expected[5], expected[6], expected[7], kore_uint16x8_get(result, 0),
		         kore_uint16x8_get(result, 1), kore_uint16x8_get(result, 2), kore_uint16x8_get(result, 3), kore_uint16x8_get(result, 4),
		         kore_uint16x8_get(result, 5), kore_uint16x8_get(result, 6), kore_uint16x8_get(result, 7));
	}
	return success;
}

static bool check_i32(const char *name, kore_int32x4 result, const int32_t expected[4]) {
	++totalests;
	bool success = true;
	for (int i = 0; i < 4; ++i) {
		if (kore_int32x4_get(result, i) != expected[i]) {
			success = false;
		}
	}
	kore_log(KORE_LOG_LEVEL_ERROR, "Test %s %s", name, success ? "PASS" : "FAIL");
	if (!success) {
		kore_log(KORE_LOG_LEVEL_INFO, "\texpected {%d, %d, %d, %d} got {%d, %d, %d, %d}", expected[0], expected[1], expected[2], expected[3],
		         kore_int32x4_get(result, 0), kore_int32x4_get(result, 1), kore_int32x4_get(result, 2), kore_int32x4_get(result, 3));
	}
	return success;
}

static bool check_u32(const char *name, kore_uint32x4 result, const uint32_t expected[4]) {
	++totalests;
	bool success = true;
	for (int i = 0; i < 4; ++i) {
		if ((kore_uint32x4_get(result, i) ^ expected[i]) != 0) {
			success = false;
		}
	}
	kore_log(KORE_LOG_LEVEL_ERROR, "Test %s %s", name, success ? "PASS" : "FAIL");
	if (!success) {
		kore_log(KORE_LOG_LEVEL_INFO, "\texpected {%lu, %lu, %lu, %lu} got {%lu, %lu, %lu, %lu}", expected[0], expected[1], expected[2], expected[3],
		         kore_uint32x4_get(result, 0), kore_uint32x4_get(result, 1), kore_uint32x4_get(result, 2), kore_uint32x4_get(result, 3));
	}
	return success;
}

int kickstart(int argc, char **argv) {
#if defined(KORE_SSE2)
	kore_log(KORE_LOG_LEVEL_INFO, "Using SSE2\n");
#elif defined(KORE_SSE)
	kore_log(KORE_LOG_LEVEL_INFO, "Using SSE for float32x4 and scalar fallback for all integer types\n");
#elif defined(KORE_NEON)
	kore_log(KORE_LOG_LEVEL_INFO, "Using NEON\n");
#else
	kore_log(KORE_LOG_LEVEL_INFO, "Using scalar fallback implementation\n");
#endif

	int failed = 0;
	{
		kore_float32x4 a = kore_float32x4_load(-1.0f, 2.0f, 3.0f, 4.0f);
		kore_float32x4 b = kore_float32x4_load_all(2.0f);

		kore_float32x4_mask mask;
		kore_float32x4      result;

		result = kore_float32x4_abs(a);
		failed += check_f32("float32x4 abs", result, (float[4]){1.0f, 2.0f, 3.0f, 4.0f}, EPSILON) ? 0 : 1;

		result = kore_float32x4_add(a, b);
		failed += check_f32("float32x4 add", result, (float[4]){1.0f, 4.0f, 5.0f, 6.0f}, EPSILON) ? 0 : 1;

		result = kore_float32x4_sub(a, b);
		failed += check_f32("float32x4 sub", result, (float[4]){-3.0f, 0.0f, 1.0f, 2.0f}, EPSILON) ? 0 : 1;

		result = kore_float32x4_mul(a, b);
		failed += check_f32("float32x4 mul", result, (float[4]){-2.0f, 4.0f, 6.0f, 8.0f}, EPSILON) ? 0 : 1;

		result = kore_float32x4_div(a, b);
		failed += check_f32("float32x4 div", result, (float[4]){-0.5f, 1.0f, 1.5f, 2.0f}, EPSILON) ? 0 : 1;

		result = kore_float32x4_neg(a);
		failed += check_f32("float32x4 neg", result, (float[4]){1.0f, -2.0f, -3.0f, -4.0f}, EPSILON) ? 0 : 1;

		result = kore_float32x4_reciprocal_approximation(a);
		failed += check_f32("float32x4 reciprocal_approximation", result, (float[4]){1.0f / -1.0f, 1.0f / 2.0f, 1.0f / 3.0f, 1.0f / 4.0f}, 0.002f) ? 0 : 1;

		result = kore_float32x4_reciprocal_sqrt_approximation(b);
		failed += check_f32("float32x4 reciprocal_sqrt_approximation", result,
		                    (float[4]){1.0f / sqrtf(2.0f), 1.0f / sqrtf(2.0f), 1.0f / sqrtf(2.0f), 1.0f / sqrtf(2.0f)}, 0.003f)
		              ? failed
		              : 1;

		result = kore_float32x4_sqrt(b);
		failed += check_f32("float32x4 sqrt", result, (float[4]){sqrtf(2.0f), sqrtf(2.0f), sqrtf(2.0f), sqrtf(2.0f)}, EPSILON) ? 0 : 1;

		result = kore_float32x4_max(a, b);
		failed += check_f32("float32x4 max", result, (float[4]){2.0f, 2.0f, 3.0f, 4.0f}, EPSILON) ? 0 : 1;

		result = kore_float32x4_min(a, b);
		failed += check_f32("float32x4 min", result, (float[4]){-1.0f, 2.0f, 2.0f, 2.0f}, EPSILON) ? 0 : 1;

		mask   = kore_float32x4_cmpeq(a, b);
		result = kore_float32x4_sel(a, b, mask);
		failed += check_f32("float32x4 cmpeq & sel", result, (float[4]){2.0f, 2.0f, 2.0f, 2.0f}, EPSILON) ? 0 : 1;

		mask   = kore_float32x4_cmpge(a, b);
		result = kore_float32x4_sel(a, b, mask);
		failed += check_f32("float32x4 cmpge & sel", result, (float[4]){2.0f, 2.0f, 3.0f, 4.0f}, EPSILON) ? 0 : 1;

		mask   = kore_float32x4_cmpgt(a, b);
		result = kore_float32x4_sel(a, b, mask);
		failed += check_f32("float32x4 cmpgt & sel", result, (float[4]){2.0f, 2.0f, 3.0f, 4.0f}, EPSILON) ? 0 : 1;

		mask   = kore_float32x4_cmple(a, b);
		result = kore_float32x4_sel(a, b, mask);
		failed += check_f32("float32x4 cmple & sel", result, (float[4]){-1.0f, 2.0f, 2.0f, 2.0f}, EPSILON) ? 0 : 1;

		mask   = kore_float32x4_cmplt(a, b);
		result = kore_float32x4_sel(a, b, mask);
		failed += check_f32("float32x4 cmplt & sel", result, (float[4]){-1.0f, 2.0f, 2.0f, 2.0f}, EPSILON) ? 0 : 1;

		mask   = kore_float32x4_cmpneq(a, b);
		result = kore_float32x4_sel(a, b, mask);
		failed += check_f32("float32x4 cmpneq & sel", result, (float[4]){-1.0f, 2.0f, 3.0f, 4.0f}, EPSILON) ? 0 : 1;
	}

	{
		kore_int8x16 a = kore_int8x16_load((int8_t[16]){-8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 3, 4, 5, 6, 7, 8});
		kore_int8x16 b = kore_int8x16_load_all(2);

		kore_int8x16_mask mask;
		kore_int8x16      result;

		result = kore_int8x16_add(a, b);
		failed += check_i8("int8x16 add", result, (int8_t[16]){-6, -5, -4, -3, -2, -1, 0, 1, 3, 4, 5, 6, 7, 8, 9, 10}) ? 0 : 1;

		result = kore_int8x16_sub(a, b);
		failed += check_i8("int8x16 sub", result, (int8_t[16]){-10, -9, -8, -7, -6, -5, -4, -3, -1, 0, 1, 2, 3, 4, 5, 6}) ? 0 : 1;

		result = kore_int8x16_max(a, b);
		failed += check_i8("int8x16 max", result, (int8_t[16]){2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 4, 5, 6, 7, 8}) ? 0 : 1;

		result = kore_int8x16_min(a, b);
		failed += check_i8("int8x16 min", result, (int8_t[16]){-8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 2, 2, 2, 2, 2, 2}) ? 0 : 1;

		mask   = kore_int8x16_cmpeq(a, b);
		result = kore_int8x16_sel(a, b, mask);
		failed += check_i8("int8x16 cmpeq & sel", result, (int8_t[16]){2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}) ? 0 : 1;

		mask   = kore_int8x16_cmpge(a, b);
		result = kore_int8x16_sel(a, b, mask);
		failed += check_i8("int8x16 cmpge & sel", result, (int8_t[16]){2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 4, 5, 6, 7, 8}) ? 0 : 1;

		mask   = kore_int8x16_cmpgt(a, b);
		result = kore_int8x16_sel(a, b, mask);
		failed += check_i8("int8x16 cmpgt & sel", result, (int8_t[16]){2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 4, 5, 6, 7, 8}) ? 0 : 1;

		mask   = kore_int8x16_cmple(a, b);
		result = kore_int8x16_sel(a, b, mask);
		failed += check_i8("int8x16 cmple & sel", result, (int8_t[16]){-8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 2, 2, 2, 2, 2, 2}) ? 0 : 1;

		mask   = kore_int8x16_cmplt(a, b);
		result = kore_int8x16_sel(a, b, mask);
		failed += check_i8("int8x16 cmplt & sel", result, (int8_t[16]){-8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 2, 2, 2, 2, 2, 2}) ? 0 : 1;

		mask   = kore_int8x16_cmpneq(a, b);
		result = kore_int8x16_sel(a, b, mask);
		failed += check_i8("int8x16 cmpneq & sel", result, (int8_t[16]){-8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 3, 4, 5, 6, 7, 8}) ? 0 : 1;

		result = kore_int8x16_or(a, b);
		failed += check_i8("int8x16 or", result,
		                   (int8_t[16]){-8 | 2, -7 | 2, -6 | 2, -5 | 2, -4 | 2, -3 | 2, -2 | 2, -1 | 2, 1 | 2, 2 | 2, 3 | 2, 4 | 2, 5 | 2, 6 | 2, 7 | 2, 8 | 2})
		              ? 0
		              : 1;

		result = kore_int8x16_and(a, b);
		failed += check_i8("int8x16 and", result,
		                   (int8_t[16]){-8 & 2, -7 & 2, -6 & 2, -5 & 2, -4 & 2, -3 & 2, -2 & 2, -1 & 2, 1 & 2, 2 & 2, 3 & 2, 4 & 2, 5 & 2, 6 & 2, 7 & 2, 8 & 2})
		              ? 0
		              : 1;

		result = kore_int8x16_xor(a, b);
		failed +=
		    check_i8("int8x16 xor", result,
		             (int8_t[16]){-8 ^ 2, -7 ^ 2, -6 ^ 2, -5 ^ 2, -4 ^ 2, -3 ^ 2, -0x2 ^ 2, -1 ^ 2, 1 ^ 2, 0x2 ^ 2, 3 ^ 2, 4 ^ 2, 5 ^ 2, 6 ^ 2, 7 ^ 2, 8 ^ 2})
		        ? 0
		        : 1;

		result = kore_int8x16_not(a);
		failed += check_i8("int8x16 not", result, (int8_t[16]){~-8, ~-7, ~-6, ~-5, ~-4, ~-3, ~-2, ~-1, ~1, ~2, ~3, ~4, ~5, ~6, ~7, ~8}) ? 0 : 1;
	}

	{
		kore_uint8x16 a = kore_uint8x16_load((uint8_t[16]){1, 2, 3, 4, 5, 6, 7, 8, 4, 2, 3, 4, 5, 6, 7, 8});
		kore_uint8x16 b = kore_uint8x16_load_all(2);

		kore_uint8x16_mask mask;
		kore_uint8x16      result;

		result = kore_uint8x16_add(a, b);
		failed += check_u8("uint8x16 add", result, (uint8_t[16]){3, 4, 5, 6, 7, 8, 9, 10, 6, 4, 5, 6, 7, 8, 9, 10}) ? 0 : 1;

		result = kore_uint8x16_sub(a, b);
		failed += check_u8("uint8x16 sub", result, (uint8_t[16]){255, 0, 1, 2, 3, 4, 5, 6, 2, 0, 1, 2, 3, 4, 5, 6}) ? 0 : 1;

		result = kore_uint8x16_max(a, b);
		failed += check_u8("uint8x16 max", result, (uint8_t[16]){2, 2, 3, 4, 5, 6, 7, 8, 4, 2, 3, 4, 5, 6, 7, 8}) ? 0 : 1;

		result = kore_uint8x16_min(a, b);
		failed += check_u8("uint8x16 min", result, (uint8_t[16]){1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}) ? 0 : 1;

		mask   = kore_uint8x16_cmpeq(a, b);
		result = kore_uint8x16_sel(a, b, mask);
		failed += check_u8("uint8x16 cmpeq & sel", result, (uint8_t[16]){2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}) ? 0 : 1;

		mask   = kore_uint8x16_cmpge(a, b);
		result = kore_uint8x16_sel(a, b, mask);
		failed += check_u8("uint8x16 cmpge & sel", result, (uint8_t[16]){2, 2, 3, 4, 5, 6, 7, 8, 4, 2, 3, 4, 5, 6, 7, 8}) ? 0 : 1;

		mask   = kore_uint8x16_cmpgt(a, b);
		result = kore_uint8x16_sel(a, b, mask);
		failed += check_u8("uint8x16 cmpgt & sel", result, (uint8_t[16]){2, 2, 3, 4, 5, 6, 7, 8, 4, 2, 3, 4, 5, 6, 7, 8}) ? 0 : 1;

		mask   = kore_uint8x16_cmple(a, b);
		result = kore_uint8x16_sel(a, b, mask);
		failed += check_u8("uint8x16 cmple & sel", result, (uint8_t[16]){1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}) ? 0 : 1;

		mask   = kore_uint8x16_cmplt(a, b);
		result = kore_uint8x16_sel(a, b, mask);
		failed += check_u8("uint8x16 cmplt & sel", result, (uint8_t[16]){1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}) ? 0 : 1;

		mask   = kore_uint8x16_cmpneq(a, b);
		result = kore_uint8x16_sel(a, b, mask);
		failed += check_u8("uint8x16 cmpneq & sel", result, (uint8_t[16]){1, 2, 3, 4, 5, 6, 7, 8, 4, 2, 3, 4, 5, 6, 7, 8}) ? 0 : 1;

		result = kore_uint8x16_or(a, b);
		failed += check_u8("uint8x16 or", result,
		                   (uint8_t[16]){1 | 2, 2 | 2, 3 | 2, 4 | 2, 5 | 2, 6 | 2, 7 | 2, 8 | 2, 4 | 2, 2 | 2, 3 | 2, 4 | 2, 5 | 2, 6 | 2, 7 | 2, 8 | 2})
		              ? 0
		              : 1;

		result = kore_uint8x16_and(a, b);
		failed += check_u8("uint8x16 and", result,
		                   (uint8_t[16]){1 & 2, 2 & 2, 3 & 2, 4 & 2, 5 & 2, 6 & 2, 7 & 2, 8 & 2, 4 & 2, 2 & 2, 3 & 2, 4 & 2, 5 & 2, 6 & 2, 7 & 2, 8 & 2})
		              ? 0
		              : 1;

		result = kore_uint8x16_xor(a, b);
		failed += check_u8("uint8x16 xor", result,
		                   (uint8_t[16]){1 ^ 2, 0x2 ^ 2, 3 ^ 2, 4 ^ 2, 5 ^ 2, 6 ^ 2, 7 ^ 2, 8 ^ 2, 4 ^ 2, 0x2 ^ 2, 3 ^ 2, 4 ^ 2, 5 ^ 2, 6 ^ 2, 7 ^ 2, 8 ^ 2})
		              ? 0
		              : 1;

		result          = kore_uint8x16_not(a);
		uint8_t chk[16] = {1, 2, 3, 4, 5, 6, 7, 8, 4, 2, 3, 4, 5, 6, 7, 8};
		for (int i = 0; i < 16; ++i)
			chk[i] = (uint8_t)(~chk[i]);
		failed += check_u8("uint8x16 not", result, chk) ? 0 : 1;
	}

	{
		kore_int16x8 a = kore_int16x8_load((int16_t[8]){-4, -3, -2, -1, 1, 2, 3, 4});
		kore_int16x8 b = kore_int16x8_load_all(2);

		kore_int16x8_mask mask;
		kore_int16x8      result;

		result = kore_int16x8_add(a, b);
		failed += check_i16("int16x8 add", result, (int16_t[8]){-2, -1, 0, 1, 3, 4, 5, 6}) ? 0 : 1;

		result = kore_int16x8_sub(a, b);
		failed += check_i16("int16x8 sub", result, (int16_t[8]){-6, -5, -4, -3, -1, 0, 1, 2}) ? 0 : 1;

		result = kore_int16x8_max(a, b);
		failed += check_i16("int16x8 max", result, (int16_t[8]){2, 2, 2, 2, 2, 2, 3, 4}) ? 0 : 1;

		result = kore_int16x8_min(a, b);
		failed += check_i16("int16x8 min", result, (int16_t[8]){-4, -3, -2, -1, 1, 2, 2, 2}) ? 0 : 1;

		mask   = kore_int16x8_cmpeq(a, b);
		result = kore_int16x8_sel(a, b, mask);
		failed += check_i16("int16x8 cmpeq & sel", result, (int16_t[8]){2, 2, 2, 2, 2, 2, 2, 2}) ? 0 : 1;

		mask   = kore_int16x8_cmpge(a, b);
		result = kore_int16x8_sel(a, b, mask);
		failed += check_i16("int16x8 cmpge & sel", result, (int16_t[8]){2, 2, 2, 2, 2, 2, 3, 4}) ? 0 : 1;

		mask   = kore_int16x8_cmpgt(a, b);
		result = kore_int16x8_sel(a, b, mask);
		failed += check_i16("int16x8 cmpgt & sel", result, (int16_t[8]){2, 2, 2, 2, 2, 2, 3, 4}) ? 0 : 1;

		mask   = kore_int16x8_cmple(a, b);
		result = kore_int16x8_sel(a, b, mask);
		failed += check_i16("int16x8 cmple & sel", result, (int16_t[8]){-4, -3, -2, -1, 1, 2, 2, 2}) ? 0 : 1;

		mask   = kore_int16x8_cmplt(a, b);
		result = kore_int16x8_sel(a, b, mask);
		failed += check_i16("int16x8 cmplt & sel", result, (int16_t[8]){-4, -3, -2, -1, 1, 2, 2, 2}) ? 0 : 1;

		mask   = kore_int16x8_cmpneq(a, b);
		result = kore_int16x8_sel(a, b, mask);
		failed += check_i16("int16x8 cmpneq & sel", result, (int16_t[8]){-4, -3, -2, -1, 1, 2, 3, 4}) ? 0 : 1;

		result = kore_int16x8_or(a, b);
		failed += check_i16("int16x8 or", result, (int16_t[8]){-4 | 2, -3 | 2, -2 | 2, -1 | 2, 1 | 2, 2 | 2, 3 | 2, 4 | 2}) ? 0 : 1;

		result = kore_int16x8_and(a, b);
		failed += check_i16("int16x8 and", result, (int16_t[8]){-4 & 2, -3 & 2, -2 & 2, -1 & 2, 1 & 2, 2 & 2, 3 & 2, 4 & 2}) ? 0 : 1;

		result = kore_int16x8_xor(a, b);
		failed += check_i16("int16x8 xor", result, (int16_t[8]){-4 ^ 2, -3 ^ 2, -0x2 ^ 2, -1 ^ 2, 1 ^ 2, 0x2 ^ 2, 3 ^ 2, 4 ^ 2}) ? 0 : 1;

		result = kore_int16x8_not(a);
		failed += check_i16("int16x8 not", result, (int16_t[8]){~-4, ~-3, ~-2, ~-1, ~1, ~2, ~3, ~4}) ? 0 : 1;
	}

	{
		kore_uint16x8 a = kore_uint16x8_load((uint16_t[8]){1, 2, 3, 4, 5, 6, 7, 8});
		kore_uint16x8 b = kore_uint16x8_load_all(2);

		kore_uint16x8_mask mask;
		kore_uint16x8      result;

		result = kore_uint16x8_add(a, b);
		failed += check_u16("uint16x8 add", result, (uint16_t[8]){3, 4, 5, 6, 7, 8, 9, 10}) ? 0 : 1;

		result = kore_uint16x8_sub(a, b);
		failed += check_u16("uint16x8 sub", result, (uint16_t[8]){65535, 0, 1, 2, 3, 4, 5, 6}) ? 0 : 1;

		mask   = kore_uint16x8_cmpeq(a, b);
		result = kore_uint16x8_sel(a, b, mask);
		failed += check_u16("uint16x8 cmpeq & sel", result, (uint16_t[8]){2, 2, 2, 2, 2, 2, 2, 2}) ? 0 : 1;

		mask   = kore_uint16x8_cmpneq(a, b);
		result = kore_uint16x8_sel(a, b, mask);
		failed += check_u16("uint16x8 cmpneq & sel", result, (uint16_t[8]){1, 2, 3, 4, 5, 6, 7, 8}) ? 0 : 1;

		result = kore_uint16x8_or(a, b);
		failed += check_u16("uint16x8 or", result, (uint16_t[8]){1 | 2, 2 | 2, 3 | 2, 4 | 2, 5 | 2, 6 | 2, 7 | 2, 8 | 2}) ? 0 : 1;

		result = kore_uint16x8_and(a, b);
		failed += check_u16("uint16x8 and", result, (uint16_t[8]){1 & 2, 2 & 2, 3 & 2, 4 & 2, 5 & 2, 6 & 2, 7 & 2, 8 & 2}) ? 0 : 1;

		result = kore_uint16x8_xor(a, b);
		failed += check_u16("uint16x8 xor", result, (uint16_t[8]){1 ^ 2, 0x2 ^ 2, 3 ^ 2, 4 ^ 2, 5 ^ 2, 6 ^ 2, 7 ^ 2, 8 ^ 2}) ? 0 : 1;

		result          = kore_uint16x8_not(a);
		uint16_t chk[8] = {1, 2, 3, 4, 5, 6, 7, 8};
		for (int i = 0; i < 8; ++i)
			chk[i] = (uint16_t)(~chk[i]);
		failed += check_u16("uint16x8 not", result, chk) ? 0 : 1;
	}

	{
		kore_int32x4 a = kore_int32x4_load((int32_t[4]){-2, -1, 1, 2});
		kore_int32x4 b = kore_int32x4_load_all(2);

		kore_int32x4_mask mask;
		kore_int32x4      result;

		result = kore_int32x4_add(a, b);
		failed += check_i32("int32x4 add", result, (int32_t[4]){0, 1, 3, 4}) ? 0 : 1;

		result = kore_int32x4_sub(a, b);
		failed += check_i32("int32x4 sub", result, (int32_t[4]){-4, -3, -1, 0}) ? 0 : 1;

		result = kore_int32x4_max(a, b);
		failed += check_i32("int32x4 max", result, (int32_t[4]){2, 2, 2, 2}) ? 0 : 1;

		result = kore_int32x4_min(a, b);
		failed += check_i32("int32x4 min", result, (int32_t[4]){-2, -1, 1, 2}) ? 0 : 1;

		mask   = kore_int32x4_cmpeq(a, b);
		result = kore_int32x4_sel(a, b, mask);
		failed += check_i32("int32x4 cmpeq & sel", result, (int32_t[4]){2, 2, 2, 2}) ? 0 : 1;

		mask   = kore_int32x4_cmpge(a, b);
		result = kore_int32x4_sel(a, b, mask);
		failed += check_i32("int32x4 cmpge & sel", result, (int32_t[4]){2, 2, 2, 2}) ? 0 : 1;

		mask   = kore_int32x4_cmpgt(a, b);
		result = kore_int32x4_sel(a, b, mask);
		failed += check_i32("int32x4 cmpgt & sel", result, (int32_t[4]){2, 2, 2, 2}) ? 0 : 1;

		mask   = kore_int32x4_cmple(a, b);
		result = kore_int32x4_sel(a, b, mask);
		failed += check_i32("int32x4 cmple & sel", result, (int32_t[4]){-2, -1, 1, 2}) ? 0 : 1;

		mask   = kore_int32x4_cmplt(a, b);
		result = kore_int32x4_sel(a, b, mask);
		failed += check_i32("int32x4 cmplt & sel", result, (int32_t[4]){-2, -1, 1, 2}) ? 0 : 1;

		mask   = kore_int32x4_cmpneq(a, b);
		result = kore_int32x4_sel(a, b, mask);
		failed += check_i32("int32x4 cmpneq & sel", result, (int32_t[4]){-2, -1, 1, 2}) ? 0 : 1;

		result = kore_int32x4_or(a, b);
		failed += check_i32("int32x4 or", result, (int32_t[4]){-2 | 2, -1 | 2, 1 | 2, 2 | 2}) ? 0 : 1;

		result = kore_int32x4_and(a, b);
		failed += check_i32("int32x4 and", result, (int32_t[4]){-2 & 2, -1 & 2, 1 & 2, 2 & 2}) ? 0 : 1;

		result = kore_int32x4_xor(a, b);
		failed += check_i32("int32x4 xor", result, (int32_t[4]){-0x2 ^ 2, -1 ^ 2, 1 ^ 2, 0x2 ^ 2}) ? 0 : 1;

		result = kore_int32x4_not(a);
		failed += check_i32("int32x4 not", result, (int32_t[4]){~-2, ~-1, ~1, ~2}) ? 0 : 1;
	}

	{
		kore_uint32x4 a = kore_uint32x4_load((uint32_t[4]){1, 2, 3, 4});
		kore_uint32x4 b = kore_uint32x4_load_all(2);

		kore_uint32x4_mask mask;
		kore_uint32x4      result;

		result = kore_uint32x4_add(a, b);
		failed += check_u32("uint32x4 add", result, (uint32_t[4]){3, 4, 5, 6}) ? 0 : 1;

		result = kore_uint32x4_sub(a, b);
		failed += check_u32("uint32x4 sub", result, (uint32_t[4]){4294967295, 0, 1, 2}) ? 0 : 1;

		mask   = kore_uint32x4_cmpeq(a, b);
		result = kore_uint32x4_sel(a, b, mask);
		failed += check_u32("uint32x4 cmpeq & sel", result, (uint32_t[4]){2, 2, 2, 2}) ? 0 : 1;

		mask   = kore_uint32x4_cmpneq(a, b);
		result = kore_uint32x4_sel(a, b, mask);
		failed += check_u32("uint32x4 cmpneq & sel", result, (uint32_t[4]){1, 2, 3, 4}) ? 0 : 1;

		result = kore_uint32x4_or(a, b);
		failed += check_u32("uint32x4 or", result, (uint32_t[4]){1 | 2, 2 | 2, 3 | 2, 4 | 2}) ? 0 : 1;

		result = kore_uint32x4_and(a, b);
		failed += check_u32("uint32x4 and", result, (uint32_t[4]){1 & 2, 2 & 2, 3 & 2, 4 & 2}) ? 0 : 1;

		result = kore_uint32x4_xor(a, b);
		failed += check_u32("uint32x4 xor", result, (uint32_t[4]){1 ^ 2, 0x2 ^ 2, 3 ^ 2, 4 ^ 2}) ? 0 : 1;

		result          = kore_uint32x4_not(a);
		uint32_t chk[4] = {1, 2, 3, 4};
		for (int i = 0; i < 4; ++i)
			chk[i] = (uint32_t)(~chk[i]);
		failed += check_u32("uint32x4 not", result, chk) ? 0 : 1;
	}

	if (failed) {
		kore_log(KORE_LOG_LEVEL_ERROR, "\nERROR! %d of %d test(s) failed", failed, totalests);
	}
	else {
		kore_log(KORE_LOG_LEVEL_INFO, "\nSUCCESS %d tests passed", totalests);
	}

	return failed;
}
