#ifndef KORE_MATH_RANDOM_HEADER
#define KORE_MATH_RANDOM_HEADER

#include <kinc/global.h>

/*! \file random.h
    \brief Generates values which are kind of random.
*/

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Initializes the randomizer with a seed. This is optional but helpful.
/// </summary>
/// <param name="seed">A value which should ideally be pretty random</param>
KORE_FUNC void kore_random_init(int64_t seed);

/// <summary>
/// Returns a random value.
/// </summary>
/// <returns>A random value</returns>
KORE_FUNC int64_t kore_random_get(void);

/// <summary>
/// Returns a value between 0 and max (both inclusive).
/// </summary>
/// <returns>A random value</returns>
KORE_FUNC int64_t kore_random_get_max(int64_t max);

/// <summary>
/// Returns a value between min and max (both inclusive).
/// </summary>
/// <returns>A random value</returns>
KORE_FUNC int64_t kore_random_get_in(int64_t min, int64_t max);

#ifdef __cplusplus
}
#endif

#endif
