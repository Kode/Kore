#ifndef KORE_MATH_VECTOR_HEADER
#define KORE_MATH_VECTOR_HEADER

#include "core.h"

/*! \file vector.h
    \brief Provides basic vector types.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_float2 {
	float x;
	float y;
} kore_float2;

typedef struct kore_float3 {
	float x;
	float y;
	float z;
} kore_float3;

typedef struct kore_float4 {
	float x;
	float y;
	float z;
	float w;
} kore_float4;

typedef struct kore_int2 {
	int32_t x;
	int32_t y;
} kore_int2;

typedef struct kore_int3 {
	int32_t x;
	int32_t y;
	int32_t z;
} kore_int3;

typedef struct kore_int4 {
	int32_t x;
	int32_t y;
	int32_t z;
	int32_t w;
} kore_int4;

typedef struct kore_uint2 {
	uint32_t x;
	uint32_t y;
} kore_uint2;

typedef struct kore_uint3 {
	uint32_t x;
	uint32_t y;
	uint32_t z;
} kore_uint3;

typedef struct kore_uint4 {
	uint32_t x;
	uint32_t y;
	uint32_t z;
	uint32_t w;
} kore_uint4;

#ifdef __cplusplus
}
#endif

#endif
