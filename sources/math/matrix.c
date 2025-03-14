#include <kore3/math/core.h>
#include <kore3/math/matrix.h>

#include <string.h>

float kore_matrix3x3_get(kore_matrix3x3 *matrix, int x, int y) {
	return matrix->m[x * 3 + y];
}

void kore_matrix3x3_set(kore_matrix3x3 *matrix, int x, int y, float value) {
	matrix->m[x * 3 + y] = value;
}

void kore_matrix3x3_transpose(kore_matrix3x3 *matrix) {
	kore_matrix3x3 transposed;
	for (int y = 0; y < 3; ++y) {
		for (int x = 0; x < 3; ++x) {
			kore_matrix3x3_set(&transposed, x, y, kore_matrix3x3_get(matrix, y, x));
		}
	}
	memcpy(matrix->m, transposed.m, sizeof(transposed.m));
}

kore_matrix3x3 kore_matrix3x3_identity(void) {
	kore_matrix3x3 m;
	memset(m.m, 0, sizeof(m.m));
	for (unsigned x = 0; x < 3; ++x) {
		kore_matrix3x3_set(&m, x, x, 1.0f);
	}
	return m;
}

kore_matrix3x3 kore_matrix3x3_rotation_x(float alpha) {
	kore_matrix3x3 m  = kore_matrix3x3_identity();
	float          ca = cosf(alpha);
	float          sa = sinf(alpha);
	kore_matrix3x3_set(&m, 1, 1, ca);
	kore_matrix3x3_set(&m, 2, 1, -sa);
	kore_matrix3x3_set(&m, 1, 2, sa);
	kore_matrix3x3_set(&m, 2, 2, ca);
	return m;
}

kore_matrix3x3 kore_matrix3x3_rotation_y(float alpha) {
	kore_matrix3x3 m  = kore_matrix3x3_identity();
	float          ca = cosf(alpha);
	float          sa = sinf(alpha);
	kore_matrix3x3_set(&m, 0, 0, ca);
	kore_matrix3x3_set(&m, 2, 0, sa);
	kore_matrix3x3_set(&m, 0, 2, -sa);
	kore_matrix3x3_set(&m, 2, 2, ca);
	return m;
}

kore_matrix3x3 kore_matrix3x3_rotation_z(float alpha) {
	kore_matrix3x3 m  = kore_matrix3x3_identity();
	float          ca = cosf(alpha);
	float          sa = sinf(alpha);
	kore_matrix3x3_set(&m, 0, 0, ca);
	kore_matrix3x3_set(&m, 1, 0, -sa);
	kore_matrix3x3_set(&m, 0, 1, sa);
	kore_matrix3x3_set(&m, 1, 1, ca);
	return m;
}

kore_matrix3x3 kore_matrix3x3_translation(float x, float y) {
	kore_matrix3x3 m = kore_matrix3x3_identity();
	kore_matrix3x3_set(&m, 2, 0, x);
	kore_matrix3x3_set(&m, 2, 1, y);
	return m;
}

kore_matrix3x3 kore_matrix3x3_scale(float x, float y, float z) {
	kore_matrix3x3 m = kore_matrix3x3_identity();
	kore_matrix3x3_set(&m, 0, 0, x);
	kore_matrix3x3_set(&m, 1, 1, y);
	kore_matrix3x3_set(&m, 2, 2, z);
	return m;
}

#ifdef __clang__
#pragma clang diagnostic ignored "-Wconditional-uninitialized"
#endif

kore_matrix3x3 kore_matrix3x3_multiply(kore_matrix3x3 *a, kore_matrix3x3 *b) {
	kore_matrix3x3 result;
	for (unsigned x = 0; x < 3; ++x) {
		for (unsigned y = 0; y < 3; ++y) {
			float t = kore_matrix3x3_get(a, 0, y) * kore_matrix3x3_get(b, x, 0);
			for (unsigned i = 1; i < 3; ++i) {
				t += kore_matrix3x3_get(a, i, y) * kore_matrix3x3_get(b, x, i);
			}
			kore_matrix3x3_set(&result, x, y, t);
		}
	}
	return result;
}

static float vector3_get(kore_float3 vec, int index) {
	float *values = (float *)&vec;
	return values[index];
}

static void vector3_set(kore_float3 *vec, int index, float value) {
	float *values = (float *)vec;
	values[index] = value;
}

kore_float3 kore_matrix3x3_multiply_vector(kore_matrix3x3 *a, kore_float3 b) {
	kore_float3 product;
	for (unsigned y = 0; y < 3; ++y) {
		float t = 0;
		for (unsigned x = 0; x < 3; ++x) {
			t += kore_matrix3x3_get(a, x, y) * vector3_get(b, x);
		}
		vector3_set(&product, y, t);
	}
	return product;
}

float kore_matrix4x4_get(kore_matrix4x4 *matrix, int x, int y) {
	return matrix->m[x * 4 + y];
}

void kore_matrix4x4_set(kore_matrix4x4 *matrix, int x, int y, float value) {
	matrix->m[x * 4 + y] = value;
}

void kore_matrix4x4_transpose(kore_matrix4x4 *matrix) {
	kore_matrix4x4 transposed;
	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 4; ++x) {
			kore_matrix4x4_set(&transposed, x, y, kore_matrix4x4_get(matrix, y, x));
		}
	}
	memcpy(matrix->m, transposed.m, sizeof(transposed.m));
}

kore_matrix4x4 kore_matrix4x4_identity(void) {
	kore_matrix4x4 m;
	memset(m.m, 0, sizeof(m.m));
	for (unsigned x = 0; x < 4; ++x) {
		kore_matrix4x4_set(&m, x, x, 1.0f);
	}
	return m;
}

kore_matrix4x4 kore_matrix4x4_rotation_x(float alpha) {
	kore_matrix4x4 m  = kore_matrix4x4_identity();
	float          ca = cosf(alpha);
	float          sa = sinf(alpha);
	kore_matrix4x4_set(&m, 1, 1, ca);
	kore_matrix4x4_set(&m, 2, 1, -sa);
	kore_matrix4x4_set(&m, 1, 2, sa);
	kore_matrix4x4_set(&m, 2, 2, ca);
	return m;
}

kore_matrix4x4 kore_matrix4x4_rotation_y(float alpha) {
	kore_matrix4x4 m  = kore_matrix4x4_identity();
	float          ca = cosf(alpha);
	float          sa = sinf(alpha);
	kore_matrix4x4_set(&m, 0, 0, ca);
	kore_matrix4x4_set(&m, 2, 0, sa);
	kore_matrix4x4_set(&m, 0, 2, -sa);
	kore_matrix4x4_set(&m, 2, 2, ca);
	return m;
}

kore_matrix4x4 kore_matrix4x4_rotation_z(float alpha) {
	kore_matrix4x4 m  = kore_matrix4x4_identity();
	float          ca = cosf(alpha);
	float          sa = sinf(alpha);
	kore_matrix4x4_set(&m, 0, 0, ca);
	kore_matrix4x4_set(&m, 1, 0, -sa);
	kore_matrix4x4_set(&m, 0, 1, sa);
	kore_matrix4x4_set(&m, 1, 1, ca);
	return m;
}

kore_matrix4x4 kore_matrix4x4_translation(float x, float y, float z) {
	kore_matrix4x4 m = kore_matrix4x4_identity();
	kore_matrix4x4_set(&m, 3, 0, x);
	kore_matrix4x4_set(&m, 3, 1, y);
	kore_matrix4x4_set(&m, 3, 2, z);
	return m;
}

kore_matrix4x4 kore_matrix4x4_scale(float x, float y, float z) {
	kore_matrix4x4 m = kore_matrix4x4_identity();
	kore_matrix4x4_set(&m, 0, 0, x);
	kore_matrix4x4_set(&m, 1, 1, y);
	kore_matrix4x4_set(&m, 2, 2, z);
	return m;
}

kore_matrix4x4 kore_matrix4x4_multiply(kore_matrix4x4 *a, kore_matrix4x4 *b) {
	kore_matrix4x4 result;
	for (unsigned x = 0; x < 4; ++x)
		for (unsigned y = 0; y < 4; ++y) {
			float t = kore_matrix4x4_get(a, 0, y) * kore_matrix4x4_get(b, x, 0);
			for (unsigned i = 1; i < 4; ++i) {
				t += kore_matrix4x4_get(a, i, y) * kore_matrix4x4_get(b, x, i);
			}
			kore_matrix4x4_set(&result, x, y, t);
		}
	return result;
}

static float vector4_get(kore_float4 vec, int index) {
	float *values = (float *)&vec;
	return values[index];
}

static void vector4_set(kore_float4 *vec, int index, float value) {
	float *values = (float *)vec;
	values[index] = value;
}

kore_float4 kore_matrix4x4_multiply_vector(kore_matrix4x4 *a, kore_float4 b) {
	kore_float4 product;
	for (unsigned y = 0; y < 4; ++y) {
		float t = 0;
		for (unsigned x = 0; x < 4; ++x) {
			t += kore_matrix4x4_get(a, x, y) * vector4_get(b, x);
		}
		vector4_set(&product, y, t);
	}
	return product;
}
