#ifndef KORE_WEBGPU_UNIT_HEADER
#define KORE_WEBGPU_UNIT_HEADER

#include <kore3/gpu/device.h>
#include <kore3/gpu/textureformat.h>
#include <kore3/webgpu/buffer_structs.h>

#include <stdint.h>

typedef struct buffer_upload {
	kore_webgpu_buffer *buffer;
	uint64_t            offset;
	uint64_t            size;
} buffer_upload;

static buffer_upload scheduled_buffer_uploads[256];
static uint32_t      scheduled_buffer_uploads_count = 0;

static WGPUCompareFunction convert_compare(kore_gpu_compare_function func);

static void kore_webgpu_buffer_schedule_uploads(kore_webgpu_buffer *buffer);

#endif
