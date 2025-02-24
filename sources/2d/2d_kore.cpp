#include "graphics.h"

#include <kore3/framebuffer/framebuffer.h>
#include <kore3/math/core.h>
#include <kore3/math/matrix.h>

#include <stdint.h>
#include <string.h>

#if 0
#include <Kore/Graphics2/Graphics.h>

static Kore::Graphics2::Graphics2 *kore_kore_g2;

static kore_matrix3x3_t transformation;

static Kore::Graphics4::Texture *tex;

void kore_g2_init(int screen_width, int screen_height) {
	tex = new Kore::Graphics4::Texture(32, 32, Kore::Graphics1::Image::RGBA32);
	kore_kore_g2 = new Kore::Graphics2::Graphics2(screen_width, screen_height);
}

void kore_g2_begin(void) {
	kore_kore_g2->begin();
}

void kore_g2_end(void) {
	kore_kore_g2->end();
}

void kore_g2_draw_image(kore_g4_texture_t *img, float x, float y) {
	tex->koreTexture = *img;
	tex->width = img->tex_width;
	tex->height = img->tex_height;
	tex->texWidth = img->tex_width;
	tex->texHeight = img->tex_height;
	kore_kore_g2->drawImage(tex, x, y);
}

void kore_g2_set_rotation(float angle, float centerx, float centery) {
	kore_kore_g2->transformation = (Kore::mat3::Translation(centerx, centery) * Kore::mat3::RotationZ(angle)) * Kore::mat3::Translation(-centerx, -centery);
}
#endif
