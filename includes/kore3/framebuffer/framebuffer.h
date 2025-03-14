#ifndef KORE_FRAMEBUFFER_HEADER
#define KORE_FRAMEBUFFER_HEADER

#include <kore3/global.h>

#include <kore3/color.h>

#include <assert.h>

/*! \file graphics.h
    \brief Supports a very basic pixel-setting API.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	KORE_FB_TEXTURE_FILTER_POINT,
	KORE_FB_TEXTURE_FILTER_LINEAR,
	KORE_FB_TEXTURE_FILTER_ANISOTROPIC,
} kore_fb_texture_filter;

typedef enum {
	KORE_FB_MIPMAP_FILTER_NONE,
	KORE_FB_MIPMAP_FILTER_POINT,
	KORE_FB_MIPMAP_FILTER_LINEAR,
} kore_fb_mipmap_filter;

/// <summary>
/// Initializes the G1-API.
/// </summary>
/// <param name="width">The width to be used by the G1-API - typically the window-width</param>
/// <param name="height">The height to be used by the G1-API - typically the window-height</param>
KORE_FUNC void kore_fb_init(int width, int height);

/// <summary>
/// Typically called once per frame before other G1-functions are called.
/// </summary>
KORE_FUNC void kore_fb_begin(void);

/// <summary>
/// Typically called once per frame after all G1-drawing is finished. This also swaps the framebuffers
/// so an equivalent call to kore_g4_swap_buffers is not needed.
/// </summary>
KORE_FUNC void kore_fb_end(void);

extern uint32_t *kore_internal_fb_image;

extern int kore_internal_fb_w;
extern int kore_internal_fb_h;
extern int kore_internal_fb_tex_width;

extern kore_fb_texture_filter kore_internal_fb_texture_filter_min;
extern kore_fb_texture_filter kore_internal_fb_texture_filter_mag;
extern kore_fb_mipmap_filter kore_internal_fb_mipmap_filter;

#if defined(KORE_DYNAMIC_COMPILE) || defined(KORE_DYNAMIC) || defined(KORE_DOCS)

/// <summary>
/// Sets a single pixel to a color.
/// </summary>
/// <param name="x">The x-component of the pixel-coordinate to set</param>
/// <param name="y">The y-component of the pixel-coordinate to set</param>
/// <param name="red">The red-component between 0 and 1</param>
/// <param name="green">The green-component between 0 and 1</param>
/// <param name="blue">The blue-component between 0 and 1</param>
KORE_FUNC void kore_fb_set_pixel(int x, int y, float red, float green, float blue);

/// <summary>
/// Returns the width used by G1.
/// </summary>
/// <returns>The width</returns>
KORE_FUNC int kore_fb_width(void);

/// <summary>
/// Returns the height used by G1.
/// </summary>
/// <returns>The height</returns>
KORE_FUNC int kore_fb_height(void);

/// <summary>
/// Set the texture-sampling-mode for upscaled textures.
/// </summary>
/// <param name="unit">The texture-unit to set the texture-sampling-mode for</param>
/// <param name="filter">The mode to set</param>
KORE_FUNC void kore_fb_set_texture_magnification_filter(kore_fb_texture_filter_t filter);

/// <summary>
/// Set the texture-sampling-mode for downscaled textures.
/// </summary>
/// <param name="unit">The texture-unit to set the texture-sampling-mode for</param>
/// <param name="filter">The mode to set</param>
KORE_FUNC void kore_fb_set_texture_minification_filter(kore_fb_texture_filter_t filter);

/// <summary>
/// Sets the mipmap-sampling-mode which defines whether mipmaps are used at all and if so whether the two neighbouring mipmaps are linearly interpolated.
/// </summary>
/// <param name="unit">The texture-unit to set the mipmap-sampling-mode for</param>
/// <param name="filter">The mode to set</param>
KORE_FUNC void kore_fb_set_texture_mipmap_filter(kore_fb_mipmap_filter_t filter);

#else

// implementation moved to the header to allow easy inlining

static inline void kore_fb_set_pixel(int x, int y, float red, float green, float blue) {
	assert(x >= 0 && x < kore_internal_fb_w && y >= 0 && y < kore_internal_fb_h);
	int r                                                      = (int)(red * 255);
	int g                                                      = (int)(green * 255);
	int b                                                      = (int)(blue * 255);
	kore_internal_fb_image[y * kore_internal_fb_tex_width + x] = 0xff << 24 | b << 16 | g << 8 | r;
}

static inline int kore_fb_width(void) {
	return kore_internal_fb_w;
}

static inline int kore_fb_height(void) {
	return kore_internal_fb_h;
}

static inline void kore_fb_set_texture_magnification_filter(kore_fb_texture_filter filter) {
	kore_internal_fb_texture_filter_mag = filter;
}

static inline void kore_fb_set_texture_minification_filter(kore_fb_texture_filter filter) {
	kore_internal_fb_texture_filter_min = filter;
}

static inline void kore_fb_set_texture_mipmap_filter(kore_fb_mipmap_filter filter) {
	kore_internal_fb_mipmap_filter = filter;
}

#endif

#ifdef __cplusplus
}
#endif

#endif
