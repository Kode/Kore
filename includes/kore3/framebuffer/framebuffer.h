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
KORE_FUNC void kore_fb_init(uint32_t width, uint32_t height);

/// <summary>
/// Typically called once per frame before other G1-functions are called.
/// </summary>
KORE_FUNC void kore_fb_begin(void);

/// <summary>
/// Typically called once per frame after all G1-drawing is finished. This also swaps the framebuffers
/// so an equivalent call to kore_g4_swap_buffers is not needed.
/// </summary>
KORE_FUNC void kore_fb_end(void);

extern uint32_t kore_framebuffer_width;
extern uint32_t kore_framebuffer_height;
extern uint32_t kore_framebuffer_stride;

extern uint8_t *kore_framebuffer_pixels;

extern kore_fb_texture_filter kore_internal_fb_texture_filter_min;
extern kore_fb_texture_filter kore_internal_fb_texture_filter_mag;
extern kore_fb_mipmap_filter  kore_internal_fb_mipmap_filter;

#if defined(KORE_DYNAMIC_COMPILE) || defined(KORE_DYNAMIC) || defined(KORE_DOCS)

/// <summary>
/// Sets a single pixel to a color.
/// </summary>
/// <param name="x">The x-component of the pixel-coordinate to set</param>
/// <param name="y">The y-component of the pixel-coordinate to set</param>
/// <param name="red">The red-component between 0 and 1</param>
/// <param name="green">The green-component between 0 and 1</param>
/// <param name="blue">The blue-component between 0 and 1</param>
KORE_FUNC void kore_fb_set_pixel(uint32_t x, uint32_t y, uint8_t red, uint8_t green, uint8_t blue);

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

static inline void kore_fb_set_pixel(uint32_t x, uint32_t y, uint8_t red, uint8_t green, uint8_t blue) {
	assert(x < kore_framebuffer_width && y < kore_framebuffer_height);
	kore_framebuffer_pixels[y * kore_framebuffer_stride + x * 4 + 0] = red;
	kore_framebuffer_pixels[y * kore_framebuffer_stride + x * 4 + 1] = green;
	kore_framebuffer_pixels[y * kore_framebuffer_stride + x * 4 + 2] = blue;
	kore_framebuffer_pixels[y * kore_framebuffer_stride + x * 4 + 3] = 255;
}

static inline uint32_t kore_fb_width(void) {
	return kore_framebuffer_width;
}

static inline uint32_t kore_fb_height(void) {
	return kore_framebuffer_height;
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
