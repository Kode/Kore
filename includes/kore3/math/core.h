#ifndef KORE_MATH_CORE_HEADER
#define KORE_MATH_CORE_HEADER

#include <kore3/global.h>

/*! \file core.h
    \brief Just a few very simple additions to math.h
*/

#ifdef __cplusplus
extern "C" {
#endif

#define KORE_PI 3.141592654
#define KORE_TAU 6.283185307

KORE_FUNC float kore_cot(float x);
KORE_FUNC float kore_round(float value);
KORE_FUNC float kore_abs(float value);
KORE_FUNC float kore_min(float a, float b);
KORE_FUNC float kore_max(float a, float b);
KORE_FUNC int kore_mini(int a, int b);
KORE_FUNC int kore_maxi(int a, int b);
KORE_FUNC float kore_clamp(float value, float minValue, float maxValue);

#ifdef __cplusplus
}
#endif

#endif
