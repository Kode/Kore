#include "openglunit.h"

#include <kore3/gpu/device.h>

#include <assert.h>

#include "buffer.c"
#include "commandlist.c"
#include "copy.c"
#include "descriptorset.c"
#include "device.c"
#include "fence.c"
#include "flip.c"
#include "pipeline.c"
#include "raytracing.c"
#include "sampler.c"
#include "texture.c"

#include <kore3/log.h>

#ifndef NDEBUG
void kore_opengl_check_errors(void) {
	GLenum code = glGetError();
	if (code != GL_NO_ERROR) {
		kore_log(KORE_LOG_LEVEL_ERROR, "GL Error %d\n", code);
		kore_debug_break();
	}
}
#endif

uint32_t kore_opengl_find_uniform_block_index(unsigned program, const char *name) {
	uint32_t value = glGetUniformBlockIndex(program, name);
	assert(value != GL_INVALID_INDEX);
	return value;
}
