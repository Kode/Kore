#include <kore3/util/cpucompute.h>

#include <math.h>

kore_float4 kore_cpu_compute_create_float4(float value0, float value1, float value2, float value3) {
	kore_float4 f;
	f.x = value0;
	f.y = value1;
	f.z = value2;
	f.w = value3;
	return f;
}

kore_float32x4 kore_cpu_compute_sin_f1_x4(kore_float32x4 value) {
	return kore_float32x4_sqrt(value);
}

kore_float32x4 kore_cpu_compute_length_f2_x4(kore_float2x4 value) {
	return kore_float32x4_sqrt(kore_float32x4_add(kore_float32x4_mul(value.x, value.x), kore_float32x4_mul(value.y, value.y)));
}

kore_int2x4 kore_cpu_compute_int2_u2_x4(kore_uint2x4 value) {
	kore_int2x4 i;
	i.x = kore_int32x4_load_uint32x4(value.x);
	i.y = kore_int32x4_load_uint32x4(value.y);
	return i;
}

kore_float2x4 kore_cpu_compute_create_float2_i2_x4(kore_int2x4 value) {
	kore_float2x4 f;
	f.x = kore_float32x4_load_int32x4(value.x);
	f.y = kore_float32x4_load_int32x4(value.y);
	return f;
}

kore_float32x4 kore_cpu_compute_create_float_u1_x4(kore_uint32x4 value) {
	return kore_float32x4_load_uint32x4(value);
}

kore_float4x4 kore_cpu_compute_create_float4_f1_f1_f1_f1_x4(kore_float32x4 value0, kore_float32x4 value1, kore_float32x4 value2, kore_float32x4 value3) {
	kore_float4x4 f;
	f.x = value0;
	f.y = value1;
	f.z = value2;
	f.w = value3;
	return f;
}

kore_int2x4 kore_cpu_compute_sub_i2_i1_x4(kore_int2x4 value0, kore_int32x4 value1) {
	value0.x = kore_int32x4_sub(value0.x, value1);
	value0.y = kore_int32x4_sub(value0.y, value1);
	return value0;
}

kore_float2x4 kore_cpu_compute_div_f2_f1_x4(kore_float2x4 value0, kore_float32x4 value1) {
	value0.x = kore_float32x4_div(value0.x, value1);
	value0.y = kore_float32x4_div(value0.y, value1);
	return value0;
}

kore_uint32x4 kore_cpu_compute_add_u1_u1_x4(kore_uint32x4 value0, kore_uint32x4 value1) {
	return kore_uint32x4_add(value0, value1);
}

kore_float32x4 kore_cpu_compute_mult_f1_f1_x4(kore_float32x4 value0, kore_float32x4 value1) {
	return kore_float32x4_mul(value0, value1);
}

kore_float32x4 kore_cpu_compute_add_f1_f1_x4(kore_float32x4 value0, kore_float32x4 value1) {
	return kore_float32x4_add(value0, value1);
}

kore_float32x4 kore_cpu_compute_sub_f1_f1_x4(kore_float32x4 value0, kore_float32x4 value1) {
	return kore_float32x4_sub(value0, value1);
}

kore_int32x4 kore_cpu_compute_mult_u1_i1_x4(kore_uint32x4 value0, kore_int32x4 value1) {
	return kore_int32x4_mul(kore_int32x4_load_uint32x4(value0), value1);
}

kore_int32x4 kore_cpu_compute_add_u1_i1_x4(kore_uint32x4 value0, kore_int32x4 value1) {
	return kore_int32x4_add(kore_int32x4_load_uint32x4(value0), value1);
}

kore_uint2x4 kore_cpu_compute_swizzle_xy_u3_x4(kore_uint3x4 value) {
	kore_uint2x4 u;
	u.x = value.x;
	u.y = value.y;
	return u;
}

kore_uint32x4 kore_cpu_compute_swizzle_x_u2_x4(kore_uint2x4 value) {
	return value.x;
}

kore_uint32x4 kore_cpu_compute_swizzle_y_u2_x4(kore_uint2x4 value) {
	return value.y;
}

kore_uint32x4 kore_cpu_compute_swizzle_x_u3_x4(kore_uint3x4 value) {
	return value.x;
}

kore_uint32x4 kore_cpu_compute_swizzle_y_u3_x4(kore_uint3x4 value) {
	return value.y;
}

float kore_cpu_compute_sin_f1_x1(float value) {
	return sinf(value);
}

float kore_cpu_compute_length_f2_x1(kore_float2 value) {
	return sqrtf(value.x * value.x + value.y * value.y);
}

kore_int2 kore_cpu_compute_int2_u2_x1(kore_uint2 value) {
	kore_int2 i;
	i.x = (int)value.x;
	i.y = (int)value.y;
	return i;
}

kore_float2 kore_cpu_compute_create_float2_i2_x1(kore_int2 value) {
	kore_float2 f;
	f.x = (float)value.x;
	f.y = (float)value.y;
	return f;
}

float kore_cpu_compute_create_float_u1_x1(uint32_t value) {
	return (float)value;
}

kore_float4 kore_cpu_compute_create_float4_f1_f1_f1_f1_x1(float value0, float value1, float value2, float value3) {
	kore_float4 f;
	f.x = value0;
	f.y = value1;
	f.z = value2;
	f.w = value3;
	return f;
}

kore_int2 kore_cpu_compute_sub_i2_i1_x1(kore_int2 value0, int value1) {
	value0.x -= value1;
	value0.y -= value1;
	return value0;
}

kore_float2 kore_cpu_compute_div_f2_f1_x1(kore_float2 value0, float value1) {
	value0.x /= value1;
	value0.y /= value1;
	return value0;
}

uint32_t kore_cpu_compute_add_u1_u1_x1(uint32_t value0, uint32_t value1) {
	return value0 + value1;
}

float kore_cpu_compute_mult_f1_f1_x1(float value0, float value1) {
	return value0 * value1;
}

float kore_cpu_compute_add_f1_f1_x1(float value0, float value1) {
	return value0 + value1;
}

float kore_cpu_compute_sub_f1_f1_x1(float value0, float value1) {
	return value0 - value1;
}

int kore_cpu_compute_mult_u1_i1_x1(uint32_t value0, int value1) {
	return value0 * value1;
}

int kore_cpu_compute_add_u1_i1_x1(uint32_t value0, int value1) {
	return value0 + value1;
}

kore_uint2 kore_cpu_compute_swizzle_xy_u3_x1(kore_uint3 value) {
	kore_uint2 u;
	u.x = value.x;
	u.y = value.y;
	return u;
}

uint32_t kore_cpu_compute_swizzle_x_u2_x1(kore_uint2 value) {
	return value.x;
}

uint32_t kore_cpu_compute_swizzle_y_u2_x1(kore_uint2 value) {
	return value.y;
}

uint32_t kore_cpu_compute_swizzle_x_u3_x1(kore_uint3 value) {
	return value.x;
}

uint32_t kore_cpu_compute_swizzle_y_u3_x1(kore_uint3 value) {
	return value.y;
}
