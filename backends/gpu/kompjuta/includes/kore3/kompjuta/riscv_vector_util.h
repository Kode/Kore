#ifndef KORE_RISCV_VECTOR_UTIL_HEADER
#define KORE_RISCV_VECTOR_UTIL_HEADER

#include <riscv_vector.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    _Alignas(128) float lane[32];
} kore_float_x32;

static inline vfloat32m1_t kore_float_x32_load(const kore_float_x32 *p) {
    size_t _vector_length = __riscv_vsetvl_e32m1(32);
    return __riscv_vle32_v_f32m1(p->lane, _vector_length);
}

static inline void vec32f_store(kore_float_x32 *p, vfloat32m1_t v) {
    size_t _vector_length = __riscv_vsetvl_e32m1(32);
    __riscv_vse32_v_f32m1(p->lane, v, _vector_length);
}

typedef struct kore_float2_x32 {
	kore_float_x32 x;
	kore_float_x32 y;
} kore_float2_x32;

typedef struct kore_float3_x32 {
	kore_float_x32 x;
	kore_float_x32 y;
	kore_float_x32 z;
} kore_float3_x32;

typedef struct kore_float4_x32 {
	kore_float_x32 x;
	kore_float_x32 y;
	kore_float_x32 z;
	kore_float_x32 w;
} kore_float4_x32;

/*typedef struct kore_int2x4 {
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
} kore_uint4x4;*/

kore_float4 kore_riscv_create_float4(float value0, float value1, float value2, float value3);

kore_float_x32 kore_riscv_sin_f1_x32(kore_float_x32 value);

kore_float_x32 kore_riscv_length_f2_x32(kore_float2_x32 value);

//kore_int2x4 kore_riscv_int2_u2_x4(kore_uint2x4 value);

//kore_float2x32 kore_riscv_create_float2_i2_x4(kore_int2x4 value);

//kore_float32x32 kore_riscv_create_float_u1_x4(kore_uint32x4 value);

static inline kore_float4_x32 kore_riscv_create_float4_f1_f1_f1_f1_x32(vfloat32m1_t value0, vfloat32m1_t value1, vfloat32m1_t value2, vfloat32m1_t value3) {
	kore_float4_x32 value;
	vec32f_store(&value.x, value0);
	vec32f_store(&value.y, value1);
	vec32f_store(&value.z, value2);
	vec32f_store(&value.w, value3);
	return value;
}

//kore_int2x4 kore_riscv_sub_i2_i1_x4(kore_int2x4 value0, kore_int32x4 value1);

kore_float2_x32 kore_riscv_div_f2_f1_x32(kore_float2_x32 value0, kore_float_x32 value1);

//kore_uint32x4 kore_riscv_add_u1_u1_x4(kore_uint32x4 value0, kore_uint32x4 value1);

kore_float_x32 kore_riscv_mult_f1_f1_x32(kore_float_x32 value0, kore_float_x32 value1);

kore_float_x32 kore_riscv_add_f1_f1_x32(kore_float_x32 value0, kore_float_x32 value1);

kore_float_x32 kore_riscv_sub_f1_f1_x32(kore_float_x32 value0, kore_float_x32 value1);

//kore_int32x4 kore_riscv_mult_u1_i1_x4(kore_uint32x4 value0, kore_int32x4 value1);

//kore_int32x4 kore_riscv_add_u1_i1_x4(kore_uint32x4 value0, kore_int32x4 value1);

//kore_uint2x4 kore_riscv_swizzle_xy_u3_x4(kore_uint3x4 value);

//kore_uint32x4 kore_riscv_swizzle_x_u2_x4(kore_uint2x4 value);

//kore_uint32x4 kore_riscv_swizzle_y_u2_x4(kore_uint2x4 value);

//kore_uint32x4 kore_riscv_swizzle_x_u3_x4(kore_uint3x4 value);

//kore_uint32x4 kore_riscv_swizzle_y_u3_x4(kore_uint3x4 value);

float kore_riscv_sin_f1_x1(float value);

float kore_riscv_length_f2_x1(kore_float2 value);

//kore_int2 kore_riscv_int2_u2_x1(kore_uint2 value);

//kore_float2 kore_riscv_create_float2_i2_x1(kore_int2 value);

//float kore_riscv_create_float_u1_x1(uint32_t value);

kore_float4 kore_riscv_create_float4_f1_f1_f1_f1_x1(float value0, float value1, float value2, float value3);

//kore_int2 kore_riscv_sub_i2_i1_x1(kore_int2 value0, int value1);

kore_float2 kore_riscv_div_f2_f1_x1(kore_float2 value0, float value1);

//uint32_t kore_riscv_add_u1_u1_x1(uint32_t value0, uint32_t value1);

float kore_riscv_mult_f1_f1_x1(float value0, float value1);

float kore_riscv_add_f1_f1_x1(float value0, float value1);

float kore_riscv_sub_f1_f1_x1(float value0, float value1);

//int kore_riscv_mult_u1_i1_x1(uint32_t value0, int value1);

//int kore_riscv_add_u1_i1_x1(uint32_t value0, int value1);

//kore_uint2 kore_riscv_swizzle_xy_u3_x1(kore_uint3 value);

//uint32_t kore_riscv_swizzle_x_u2_x1(kore_uint2 value);

//uint32_t kore_riscv_swizzle_y_u2_x1(kore_uint2 value);

//uint32_t kore_riscv_swizzle_x_u3_x1(kore_uint3 value);

//uint32_t kore_riscv_swizzle_y_u3_x1(kore_uint3 value);

#ifdef __cplusplus
}
#endif

#endif
