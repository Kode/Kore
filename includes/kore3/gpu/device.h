#ifndef KORE_GPU_DEVICE_HEADER
#define KORE_GPU_DEVICE_HEADER

#include <kore3/global.h>

#include <kore3/math/matrix.h>

#include "api.h"
#include "buffer.h"
#include "commandlist.h"
#include "fence.h"
#include "sampler.h"
#include "textureformat.h"

#if defined(KORE_DIRECT3D11)
#include <kore3/direct3d11/device_structs.h>
#elif defined(KORE_DIRECT3D12)
#include <kore3/direct3d12/device_structs.h>
#elif defined(KORE_METAL)
#include <kore3/metal/device_structs.h>
#elif defined(KORE_OPENGL)
#include <kore3/opengl/device_structs.h>
#elif defined(KORE_VULKAN)
#include <kore3/vulkan/device_structs.h>
#elif defined(KORE_WEBGPU)
#include <kore3/webgpu/device_structs.h>
#else
#error("Unknown GPU backend")
#endif

/*! \file device.h
    \brief The device is the entry point for doing anything with the GPU.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_gpu_device_wishlist {
	int nothing;
} kore_gpu_device_wishlist;

typedef struct kore_gpu_device {
	KORE_GPU_IMPL(device);
} kore_gpu_device;

KORE_FUNC void kore_gpu_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist);

KORE_FUNC void kore_gpu_device_destroy(kore_gpu_device *device);

KORE_FUNC void kore_gpu_device_set_name(kore_gpu_device *device, const char *name);

typedef enum kore_gpu_buffer_usage {
	KORE_GPU_BUFFER_USAGE_CPU_READ          = 0x0001,
	KORE_GPU_BUFFER_USAGE_CPU_WRITE         = 0x0002,
	KORE_GPU_BUFFER_USAGE_COPY_SRC          = 0x0004,
	KORE_GPU_BUFFER_USAGE_COPY_DST          = 0x0008,
	KORE_GPU_BUFFER_USAGE_INDEX             = 0x0010,
	KORE_GPU_BUFFER_USAGE_VERTEX            = 0x0020,
	KORE_GPU_BUFFER_USAGE_READ_WRITE        = 0x0040,
	KORE_GPU_BUFFER_USAGE_INDIRECT          = 0x0080,
	KORE_GPU_BUFFER_USAGE_QUERY_RESOLVE     = 0x0100,
	KORE_GPU_BUFFER_USAGE_RAYTRACING_VOLUME = 0x0200
} kore_gpu_buffer_usage;

typedef struct kore_gpu_buffer_parameters {
	uint64_t size;
	uint32_t usage_flags;
} kore_gpu_buffer_parameters;

KORE_FUNC void kore_gpu_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer);

typedef enum kore_gpu_texture_dimension {
	KORE_GPU_TEXTURE_DIMENSION_1D,
	KORE_GPU_TEXTURE_DIMENSION_2D,
	KORE_GPU_TEXTURE_DIMENSION_3D
} kore_gpu_texture_dimension;

typedef enum kore_gpu_texture_usage {
	KONG_G5_TEXTURE_USAGE_COPY_SRC          = 0x01,
	KONG_G5_TEXTURE_USAGE_COPY_DST          = 0x02,
	KONG_G5_TEXTURE_USAGE_SAMPLE            = 0x04,
	KONG_G5_TEXTURE_USAGE_READ_WRITE        = 0x08,
	KONG_G5_TEXTURE_USAGE_RENDER_ATTACHMENT = 0x10,
	KONG_G5_TEXTURE_USAGE_FRAMEBUFFER       = 0x20
} kore_gpu_texture_usage;

typedef struct kore_gpu_texture_parameters {
	uint32_t width;
	uint32_t height;
	uint32_t depth_or_array_layers;
	uint32_t mip_level_count;
	uint32_t sample_count;
	kore_gpu_texture_dimension dimension;
	kore_gpu_texture_format format;
	kore_gpu_texture_usage usage;
	kore_gpu_texture_format view_formats[8]; // necessary?
} kore_gpu_texture_parameters;

KORE_FUNC void kore_gpu_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture);

#define KORE_GPU_MAX_FRAMEBUFFERS 3

KORE_FUNC kore_gpu_texture *kore_gpu_device_get_framebuffer(kore_gpu_device *device);

KORE_FUNC kore_gpu_texture_format kore_gpu_device_framebuffer_format(kore_gpu_device *device);

typedef enum kore_gpu_address_mode {
	KORE_GPU_ADDRESS_MODE_CLAMP_TO_EDGE,
	KORE_GPU_ADDRESS_MODE_REPEAT,
	KORE_GPU_ADDRESS_MODE_MIRROR_REPEAT
} kore_gpu_address_mode;

typedef enum kore_gpu_filter_mode { KORE_GPU_FILTER_MODE_NEAREST, KORE_GPU_FILTER_MODE_LINEAR } kore_gpu_filter_mode;

typedef enum kore_gpu_mipmap_filter_mode { KORE_GPU_MIPMAP_FILTER_MODE_NEAREST, KORE_GPU_MIPMAP_FILTER_MODE_LINEAR } kore_gpu_mipmap_filter_mode;

typedef enum kore_gpu_compare_function {
	KORE_GPU_COMPARE_FUNCTION_NEVER,
	KORE_GPU_COMPARE_FUNCTION_LESS,
	KORE_GPU_COMPARE_FUNCTION_EQUAL,
	KORE_GPU_COMPARE_FUNCTION_LESS_EQUAL,
	KORE_GPU_COMPARE_FUNCTION_GREATER,
	KORE_GPU_COMPARE_FUNCTION_NOT_EQUAL,
	KORE_GPU_COMPARE_FUNCTION_GREATER_EQUAL,
	KORE_GPU_COMPARE_FUNCTION_ALWAYS
} kore_gpu_compare_function;

typedef struct kore_gpu_sampler_parameters {
	kore_gpu_address_mode address_mode_u;
	kore_gpu_address_mode address_mode_v;
	kore_gpu_address_mode address_mode_w;
	kore_gpu_filter_mode mag_filter;
	kore_gpu_filter_mode min_filter;
	kore_gpu_mipmap_filter_mode mipmap_filter;
	float lod_min_clamp;
	float lod_max_clamp;
	kore_gpu_compare_function compare;
	uint16_t max_anisotropy;
} kore_gpu_sampler_parameters;

KORE_FUNC void kore_gpu_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler);

typedef enum kore_gpu_command_list_type {
	KORE_GPU_COMMAND_LIST_TYPE_GRAPHICS,
	KORE_GPU_COMMAND_LIST_TYPE_COMPUTE,
	KORE_GPU_COMMAND_LIST_TYPE_COPY
} kore_gpu_command_list_type;

KORE_FUNC void kore_gpu_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list);

typedef struct kore_gpu_query_set {
	KORE_GPU_IMPL(query_set);
} kore_gpu_query_set;

typedef enum kore_gpu_query_type { KORE_GPU_QUERY_TYPE_OCCLUSION, KORE_GPU_QUERY_TYPE_TIMESTAMP } kore_gpu_query_type;

typedef struct kore_gpu_query_set_parameters {
	kore_gpu_query_type type;
	uint32_t count;
} kore_gpu_query_set_parameters;

KORE_FUNC void kore_gpu_device_create_query_set(kore_gpu_device *device, const kore_gpu_query_set_parameters *parameters, kore_gpu_query_set *query_set);

KORE_FUNC void kore_gpu_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence);

KORE_FUNC void kore_gpu_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list);

KORE_FUNC void kore_gpu_device_wait_until_idle(kore_gpu_device *device);

typedef struct kore_gpu_raytracing_volume {
	KORE_GPU_IMPL(raytracing_volume);
} kore_gpu_raytracing_volume;

typedef struct kore_gpu_raytracing_hierarchy {
	KORE_GPU_IMPL(raytracing_hierarchy);
} kore_gpu_raytracing_hierarchy;

KORE_FUNC void kore_gpu_device_create_raytracing_volume(kore_gpu_device *device, kore_gpu_buffer *vertex_buffer, uint64_t vertex_count,
                                                        kore_gpu_buffer *index_buffer, uint32_t index_count, kore_gpu_raytracing_volume *volume);

KORE_FUNC void kore_gpu_device_create_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_volume **volumes, kore_matrix4x4 *volume_transforms,
                                                           uint32_t volumes_count, kore_gpu_raytracing_hierarchy *hierarchy);

KORE_FUNC uint32_t kore_gpu_device_align_texture_row_bytes(kore_gpu_device *device, uint32_t row_bytes);

KORE_FUNC void kore_gpu_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value);

KORE_FUNC void kore_gpu_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value);

#ifdef __cplusplus
}
#endif

#endif
