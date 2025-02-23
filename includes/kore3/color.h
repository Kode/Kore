#ifndef KORE_COLOR_HEADER
#define KORE_COLOR_HEADER

#include <kore3/global.h>

#include <stdint.h>

/*! \file color.h
    \brief Provides some utility functionality for handling 32 bit ARGB color values.
*/

#ifdef __cplusplus
extern "C" {
#endif

/// <summary>
/// Splits up a 32 bit ARGB color value into its components.
/// </summary>
KORE_FUNC void kore_color_components(uint32_t color, float *red, float *green, float *blue, float *alpha);

#define KORE_COLOR_BLACK 0xff000000
#define KORE_COLOR_WHITE 0xffffffff
#define KORE_COLOR_RED 0xffff0000
#define KORE_COLOR_BLUE 0xff0000ff
#define KORE_COLOR_GREEN 0xff00ff00
#define KORE_COLOR_MAGENTA 0xffff00ff
#define KORE_COLOR_YELLOW 0xffffff00
#define KORE_COLOR_CYAN 0xff00ffff

#ifdef __cplusplus
}
#endif

#endif
