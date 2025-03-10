#ifndef KORE_UTIL_CPUCOMPUTE_HEADER
#define KORE_UTIL_CPUCOMPUTE_HEADER

#include <kore3/math/vector.h>

#ifdef __cplusplus
extern "C" {
#endif

float kore_cpu_compute_sin_f1(float value);

float kore_cpu_compute_length_f2(kore_float2 value);

kore_int2 kore_cpu_compute_int2_u2(kore_uint2 value);

kore_float2 kore_cpu_compute_create_float2_i2(kore_int2 value);

float kore_cpu_compute_create_float_u1(uint32_t value);

kore_float4 kore_cpu_compute_create_float4_f1_f1_f1_f1(float value0, float value1, float value2, float value3);

#ifdef __cplusplus
}
#endif

#endif
