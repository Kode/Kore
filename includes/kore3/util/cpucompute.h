#ifndef KORE_UTIL_CPUCOMPUTE_HEADER
#define KORE_UTIL_CPUCOMPUTE_HEADER

#include <kore3/math/vector.h>
#include <kore3/simd/float32x4.h>
#include <kore3/simd/int32x4.h>
#include <kore3/simd/uint32x4.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_float2x4 {
	kore_float32x4 x;
	kore_float32x4 y;
} kore_float2x4;

typedef struct kore_float3x4 {
	kore_float32x4 x;
	kore_float32x4 y;
	kore_float32x4 z;
} kore_float3x4;

typedef struct kore_float4x4 {
	kore_float32x4 x;
	kore_float32x4 y;
	kore_float32x4 z;
	kore_float32x4 w;
} kore_float4x4;

typedef struct kore_int2x4 {
	kore_int32x4 x;
	kore_int32x4 y;
} kore_int2x4;

typedef struct kore_int3x4 {
	kore_int32x4 x;
	kore_int32x4 y;
	kore_int32x4 z;
} kore_int3x4;

typedef struct kore_int4x4 {
	kore_int32x4 x;
	kore_int32x4 y;
	kore_int32x4 z;
	kore_int32x4 w;
} kore_int4x4;

typedef struct kore_uint2x4 {
	kore_uint32x4 x;
	kore_uint32x4 y;
} kore_uint2x4;

typedef struct kore_uint3x4 {
	kore_uint32x4 x;
	kore_uint32x4 y;
	kore_uint32x4 z;
} kore_uint3x4;

typedef struct kore_uint4x4 {
	kore_uint32x4 x;
	kore_uint32x4 y;
	kore_uint32x4 z;
	kore_uint32x4 w;
} kore_uint4x4;

kore_float4 kore_cpu_compute_create_float4(float value0, float value1, float value2, float value3);

kore_float32x4 kore_cpu_compute_sin_f1_x4(kore_float32x4 value);

kore_float32x4 kore_cpu_compute_length_f2_x4(kore_float2x4 value);

kore_int2x4 kore_cpu_compute_int2_u2_x4(kore_uint2x4 value);

kore_float2x4 kore_cpu_compute_create_float2_i2_x4(kore_int2x4 value);

kore_float32x4 kore_cpu_compute_create_float_u1_x4(kore_uint32x4 value);

kore_float4x4 kore_cpu_compute_create_float4_f1_f1_f1_f1_x4(kore_float32x4 value0, kore_float32x4 value1, kore_float32x4 value2, kore_float32x4 value3);

kore_int2x4 kore_cpu_compute_sub_i2_i1_x4(kore_int2x4 value0, kore_int32x4 value1);

kore_float2x4 kore_cpu_compute_div_f2_f1_x4(kore_float2x4 value0, kore_float32x4 value1);

kore_uint32x4 kore_cpu_compute_add_u1_u1_x4(kore_uint32x4 value0, kore_uint32x4 value1);

kore_float32x4 kore_cpu_compute_mult_f1_f1_x4(kore_float32x4 value0, kore_float32x4 value1);

kore_float32x4 kore_cpu_compute_add_f1_f1_x4(kore_float32x4 value0, kore_float32x4 value1);

kore_float32x4 kore_cpu_compute_sub_f1_f1_x4(kore_float32x4 value0, kore_float32x4 value1);

kore_int32x4 kore_cpu_compute_mult_u1_i1_x4(kore_uint32x4 value0, kore_int32x4 value1);

kore_int32x4 kore_cpu_compute_add_u1_i1_x4(kore_uint32x4 value0, kore_int32x4 value1);

kore_uint2x4 kore_cpu_compute_swizzle_xy_u3_x4(kore_uint3x4 value);

kore_uint32x4 kore_cpu_compute_swizzle_x_u2_x4(kore_uint2x4 value);

kore_uint32x4 kore_cpu_compute_swizzle_y_u2_x4(kore_uint2x4 value);

kore_uint32x4 kore_cpu_compute_swizzle_x_u3_x4(kore_uint3x4 value);

kore_uint32x4 kore_cpu_compute_swizzle_y_u3_x4(kore_uint3x4 value);

float kore_cpu_compute_sin_f1_x1(float value);

float kore_cpu_compute_length_f2_x1(kore_float2 value);

kore_int2 kore_cpu_compute_int2_u2_x1(kore_uint2 value);

kore_float2 kore_cpu_compute_create_float2_i2_x1(kore_int2 value);

float kore_cpu_compute_create_float_u1_x1(uint32_t value);

kore_float4 kore_cpu_compute_create_float4_f1_f1_f1_f1_x1(float value0, float value1, float value2, float value3);

kore_int2 kore_cpu_compute_sub_i2_i1_x1(kore_int2 value0, int value1);

kore_float2 kore_cpu_compute_div_f2_f1_x1(kore_float2 value0, float value1);

uint32_t kore_cpu_compute_add_u1_u1_x1(uint32_t value0, uint32_t value1);

float kore_cpu_compute_mult_f1_f1_x1(float value0, float value1);

float kore_cpu_compute_add_f1_f1_x1(float value0, float value1);

float kore_cpu_compute_sub_f1_f1_x1(float value0, float value1);

int kore_cpu_compute_mult_u1_i1_x1(uint32_t value0, int value1);

int kore_cpu_compute_add_u1_i1_x1(uint32_t value0, int value1);

kore_uint2 kore_cpu_compute_swizzle_xy_u3_x1(kore_uint3 value);

uint32_t kore_cpu_compute_swizzle_x_u2_x1(kore_uint2 value);

uint32_t kore_cpu_compute_swizzle_y_u2_x1(kore_uint2 value);

uint32_t kore_cpu_compute_swizzle_x_u3_x1(kore_uint3 value);

uint32_t kore_cpu_compute_swizzle_y_u3_x1(kore_uint3 value);

#ifdef __cplusplus
}
#endif

#endif
