#include "openglunit.h"

#include <kore3/gpu/device.h>

#include <assert.h>

#include "buffer.c"
#include "commandlist.c"
#include "descriptorset.c"
#include "device.c"
#include "flip.c"
#include "pipeline.c"
#include "sampler.c"
#include "texture.c"

#include <kore3/log.h>

#ifndef NDEBUG
void kore_opengl_check_errors(void) {
	GLenum code = glGetError();
	if (code != GL_NO_ERROR) {
		kore_log(KORE_LOG_LEVEL_ERROR, "GL Error %d %s %d\n", code, __FILE__, __LINE__);
	}
}
#endif
