#ifndef KORE_MINICLIB_MATH_HEADER
#define KORE_MINICLIB_MATH_HEADER

#ifdef __cplusplus
extern "C" {
#endif

double ldexp(double x, int exp);

double pow(double base, double exponent);

double floor(double x);

float floorf(float x);

double sin(double x);

float sinf(float x);

double cos(double x);

float cosf(float x);

double tan(double x);

float tanf(float x);

double log(double x);

double exp(double x);

double sqrt(double x);

float sqrtf(float x);

#ifdef __cplusplus
}
#endif

#endif
