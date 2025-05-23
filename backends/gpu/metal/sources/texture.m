#include <kore3/metal/texture_functions.h>

#include "metalunit.h"

#include <kore3/gpu/texture.h>

void kore_metal_texture_set_name(kore_gpu_texture *texture, const char *name) {}

void kore_metal_texture_destroy(kore_gpu_texture *texture) {
	CFRelease(texture->metal.texture);
	texture->metal.texture = NULL;
}
