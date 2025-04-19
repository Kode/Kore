#ifndef KORE_WEBGPU_UNIT_HEADER
#define KORE_WEBGPU_UNIT_HEADER

#include <kore3/webgpu/buffer_structs.h>
#include <kore3/gpu/textureformat.h>

static kore_webgpu_buffer *scheduled_buffer_uploads[256];
static uint32_t            scheduled_buffer_uploads_count = 0;

static WGPUTextureFormat convert_format(kore_gpu_texture_format format);

#endif
