#include <kore3/util/cpucompute.h>

#include <math.h>

float kore_cpu_compute_sin_f1(float value) {
	return sinf(value);
}

float kore_cpu_compute_length_f2(kore_float2 value) {
	return sqrtf(value.x * value.x + value.y * value.y);
}

kore_int2 kore_cpu_compute_int2_u2(kore_uint2 value) {
	kore_int2 i;
	i.x = (int)value.x;
	i.y = (int)value.y;
	return i;
}

kore_float2 kore_cpu_compute_create_float2_i2(kore_int2 value) {
	kore_float2 f;
	f.x = (float)value.x;
	f.y = (float)value.y;
	return f;
}

float kore_cpu_compute_create_float_u1(uint32_t value) {
	return (float)value;
}

kore_float4 kore_cpu_compute_create_float4_f1_f1_f1_f1(float value0, float value1, float value2, float value3) {
	kore_float4 f;
	f.x = value0;
	f.y = value1;
	f.z = value2;
	f.w = value3;
	return f;
}
