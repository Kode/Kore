#ifndef KORE_2D_HEADER
#define KORE_2D_HEADER

#include <kore3/global.h>

#if 0

#include <kore3/graphics4/texture.h>
#include <kore3/math/matrix.h>

/*! \file graphics.h
    \brief This is still in progress, please don't use it.
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef KORE_KONG

typedef enum kore_g2_image_scale_quality {
	KORE_G2_IMAGE_SCALE_QUALITY_LOW, // usually point filter
	KORE_G2_IMAGE_SCALE_QUALITY_HIGH // usually bilinear filter
} kore_g2_image_scale_quality;

// enum HorTextAlignment { TextLeft, TextCenter, TextRight };
// enum VerTextAlignment { TextTop, TextMiddle, TextBottom };

void kore_g2_init(void);
void kore_g2_draw_image(kore_g4_texture_t *img, float x, float y);
void kore_g2_draw_scaled_sub_image(kore_g4_texture_t *img, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh);
void kore_g2_draw_sub_image(kore_g4_texture_t *img, float x, float y, float sx, float sy, float sw, float sh);
void kore_g2_draw_scaled_image(kore_g4_texture_t *img, float dx, float dy, float dw, float dh);
uint32_t kore_g2_get_color(void);
void kore_g2_set_color(uint32_t color);
void kore_g2_draw_rect(float x, float y, float width, float height, float strength /*= 1.0*/);
void kore_g2_fill_rect(float x, float y, float width, float height);
void kore_g2_draw_line(float x1, float y1, float x2, float y2, float strength /*= 1.0*/);
void kore_g2_fill_triangle(float x1, float y1, float x2, float y2, float x3, float y3);
kore_g2_image_scale_quality kore_g2_get_image_scale_quality(void);
void kore_g2_set_image_scale_quality(kore_g2_image_scale_quality value);
kore_g2_image_scale_quality kore_g2_get_mipmap_scale_quality(void);
void kore_g2_set_mipmap_scale_quality(kore_g2_image_scale_quality value);
void kore_g2_scissor(int x, int y, int width, int height);
void kore_g2_disable_scissor(void);
void kore_g2_begin(int width, int height, bool clear /*= true*/, uint32_t clear_color);
void kore_g2_clear(uint32_t color);
void kore_g2_flush(void);
void kore_g2_end(void);
kore_matrix3x3_t kore_g2_get_transformation(void);
void kore_g2_set_transformation(kore_matrix3x3_t transformation);
void kore_g2_push_transformation(kore_matrix3x3_t trans);
kore_matrix3x3_t kore_g2_pop_transformation(void);
void kore_g2_scale(float x, float y);
void kore_g2_push_scale(float x, float y);
kore_matrix3x3_t kore_g2_translation(float tx, float ty);
void kore_g2_translate(float tx, float ty);
void kore_g2_push_translation(float tx, float ty);
kore_matrix3x3_t kore_g2_rotation(float angle, float centerx, float centery);
void kore_g2_rotate(float angle, float centerx, float centery);
void kore_g2_push_rotation(float angle, float centerx, float centery);
void kore_g2_push_opacity(float opacity);
float kore_g2_pop_opacity(void);
float kore_g2_get_opacity(void);
void kore_g2_set_opacity(float opacity);

#else

void kore_g2_init(int screen_width, int screen_height);
void kore_g2_begin(void);
void kore_g2_end(void);
void kore_g2_clear(float r, float g, float b);
void kore_g2_draw_image(kore_image_t *img, float x, float y);
// void drawScaledSubImage(Graphics4::Texture *img, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh);
void kore_g2_set_rotation(float angle, float centerx, float centery);

#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
