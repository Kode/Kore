#ifndef KORE_MATH_MATRIX_HEADER
#define KORE_MATH_MATRIX_HEADER

#include "vector.h"

/*! \file matrix.h
    \brief Provides basic matrix types and a few functions which create transformation-matrices. This only provides functionality which is needed elsewhere in
   Kore - if you need more, look up how transformation-matrices work and add some functions to your own project. Alternatively the Kore/C++-API also provides a
   more complete matrix-API.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_matrix3x3 {
	float m[3 * 3];
} kore_matrix3x3;

KORE_FUNC float          kore_matrix3x3_get(kore_matrix3x3 *matrix, int x, int y);
KORE_FUNC void           kore_matrix3x3_set(kore_matrix3x3 *matrix, int x, int y, float value);
KORE_FUNC void           kore_matrix3x3_transpose(kore_matrix3x3 *matrix);
KORE_FUNC kore_matrix3x3 kore_matrix3x3_identity(void);
KORE_FUNC kore_matrix3x3 kore_matrix3x3_rotation_x(float alpha);
KORE_FUNC kore_matrix3x3 kore_matrix3x3_rotation_y(float alpha);
KORE_FUNC kore_matrix3x3 kore_matrix3x3_rotation_z(float alpha);
KORE_FUNC kore_matrix3x3 kore_matrix3x3_translation(float x, float y);
KORE_FUNC kore_matrix3x3 kore_matrix3x3_scale(float x, float y, float z);
KORE_FUNC kore_matrix3x3 kore_matrix3x3_multiply(kore_matrix3x3 *a, kore_matrix3x3 *b);
KORE_FUNC kore_float3    kore_matrix3x3_multiply_vector(kore_matrix3x3 *a, kore_float3 b);

typedef struct kore_matrix4x4 {
	float m[4 * 4];
} kore_matrix4x4;

KORE_FUNC float          kore_matrix4x4_get(kore_matrix4x4 *matrix, int x, int y);
KORE_FUNC void           kore_matrix4x4_set(kore_matrix4x4 *matrix, int x, int y, float value);
KORE_FUNC void           kore_matrix4x4_transpose(kore_matrix4x4 *matrix);
KORE_FUNC kore_matrix4x4 kore_matrix4x4_identity(void);
KORE_FUNC kore_matrix4x4 kore_matrix4x4_rotation_x(float alpha);
KORE_FUNC kore_matrix4x4 kore_matrix4x4_rotation_y(float alpha);
KORE_FUNC kore_matrix4x4 kore_matrix4x4_rotation_z(float alpha);
KORE_FUNC kore_matrix4x4 kore_matrix4x4_translation(float x, float y, float z);
KORE_FUNC kore_matrix4x4 kore_matrix4x4_scale(float x, float y, float z);
KORE_FUNC kore_matrix4x4 kore_matrix4x4_multiply(kore_matrix4x4 *a, kore_matrix4x4 *b);
KORE_FUNC kore_float4    kore_matrix4x4_multiply_vector(kore_matrix4x4 *a, kore_float4 b);

#ifdef __cplusplus
}
#endif

#endif
