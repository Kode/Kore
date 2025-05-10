#include <kore3/opengl/texture_functions.h>

#include "openglunit.h"

#include <kore3/gpu/texture.h>

void kore_opengl_texture_set_name(kore_gpu_texture *texture, const char *name) {}

void kore_opengl_texture_destroy(kore_gpu_texture *texture) {
	glDeleteTextures(1, &texture->opengl.texture);
}
