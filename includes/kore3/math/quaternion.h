#ifndef KORE_MATH_QUATERNION_HEADER
#define KORE_MATH_QUATERNION_HEADER

#include "core.h"

/*! \file quaternion.h
    \brief Provides a basic quaternion type.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_quaternion {
	float x;
	float y;
	float z;
	float w;
} kore_quaternion;

#ifdef __cplusplus
}
#endif

#endif
