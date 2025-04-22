#ifndef KORE_WEBGPU_PIPELINE_STRUCTS_HEADER
#define KORE_WEBGPU_PIPELINE_STRUCTS_HEADER

#include <kore3/gpu/buffer.h>
#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/device.h>
#include <kore3/gpu/texture.h>

#include "webgpu.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum kore_webgpu_vertex_step_mode { KORE_WEBGPU_VERTEX_STEP_MODE_VERTEX, KORE_WEBGPU_VERTEX_STEP_MODE_INSTANCE } kore_webgpu_vertex_step_mode;

typedef enum kore_webgpu_vertex_format {
	KORE_WEBGPU_VERTEX_FORMAT_UINT8X2,
	KORE_WEBGPU_VERTEX_FORMAT_UINT8X4,
	KORE_WEBGPU_VERTEX_FORMAT_SINT8X2,
	KORE_WEBGPU_VERTEX_FORMAT_SINT8X4,
	KORE_WEBGPU_VERTEX_FORMAT_UNORM8X2,
	KORE_WEBGPU_VERTEX_FORMAT_UNORM8X4,
	KORE_WEBGPU_VERTEX_FORMAT_SNORM8X2,
	KORE_WEBGPU_VERTEX_FORMAT_SNORM8X4,
	KORE_WEBGPU_VERTEX_FORMAT_UINT16X2,
	KORE_WEBGPU_VERTEX_FORMAT_UINT16X4,
	KORE_WEBGPU_VERTEX_FORMAT_SINT16X2,
	KORE_WEBGPU_VERTEX_FORMAT_SINT16X4,
	KORE_WEBGPU_VERTEX_FORMAT_UNORM16X2,
	KORE_WEBGPU_VERTEX_FORMAT_UNORM16X4,
	KORE_WEBGPU_VERTEX_FORMAT_SNORM16X2,
	KORE_WEBGPU_VERTEX_FORMAT_SNORM16X4,
	KORE_WEBGPU_VERTEX_FORMAT_FLOAT16X2,
	KORE_WEBGPU_VERTEX_FORMAT_FLOAT16X4,
	KORE_WEBGPU_VERTEX_FORMAT_FLOAT32,
	KORE_WEBGPU_VERTEX_FORMAT_FLOAT32X2,
	KORE_WEBGPU_VERTEX_FORMAT_FLOAT32X3,
	KORE_WEBGPU_VERTEX_FORMAT_FLOAT32X4,
	KORE_WEBGPU_VERTEX_FORMAT_UINT32,
	KORE_WEBGPU_VERTEX_FORMAT_UINT32X2,
	KORE_WEBGPU_VERTEX_FORMAT_UINT32X3,
	KORE_WEBGPU_VERTEX_FORMAT_UINT32X4,
	KORE_WEBGPU_VERTEX_FORMAT_SIN32,
	KORE_WEBGPU_VERTEX_FORMAT_SINT32X2,
	KORE_WEBGPU_VERTEX_FORMAT_SINT32X3,
	KORE_WEBGPU_VERTEX_FORMAT_SINT32X4,
	KORE_WEBGPU_VERTEX_FORMAT_UNORM10_10_10_2
} kore_webgpu_vertex_format;

typedef struct kore_webgpu_vertex_attribute {
	kore_webgpu_vertex_format format;
	uint64_t                  offset;
	uint32_t                  shader_location;
} kore_webgpu_vertex_attribute;

#define KORE_WEBGPU_MAX_VERTEX_ATTRIBUTES 32

typedef struct kore_webgpu_vertex_buffer_layout {
	uint64_t                     array_stride;
	kore_webgpu_vertex_step_mode step_mode;
	kore_webgpu_vertex_attribute attributes[KORE_WEBGPU_MAX_VERTEX_ATTRIBUTES];
	size_t                       attributes_count;
} kore_webgpu_vertex_buffer_layout;

typedef struct kore_webgpu_shader {
	const char *function_name;
} kore_webgpu_shader;

#define KORE_WEBGPU_MAX_VERTEX_BUFFERS 16

typedef struct kore_webgpu_vertex_state {
	kore_webgpu_shader               shader;
	kore_webgpu_vertex_buffer_layout buffers[KORE_WEBGPU_MAX_VERTEX_BUFFERS];
	size_t                           buffers_count;
} kore_webgpu_vertex_state;

typedef enum kore_webgpu_primitive_topology {
	KORE_WEBGPU_PRIMITIVE_TOPOLOGY_POINT_LIST,
	KORE_WEBGPU_PRIMITIVE_TOPOLOGY_LINE_LIST,
	KORE_WEBGPU_PRIMITIVE_TOPOLOGY_LINE_STRIP,
	KORE_WEBGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	KORE_WEBGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
} kore_webgpu_primitive_topology;

typedef enum kore_webgpu_front_face { KORE_WEBGPU_FRONT_FACE_CCW, KORE_WEBGPU_FRONT_FACE_CW } kore_webgpu_front_face;

typedef enum kore_webgpu_cull_mode { KORE_WEBGPU_CULL_MODE_NONE, KORE_WEBGPU_CULL_MODE_FRONT, KORE_WEBGPU_CULL_MODE_BACK } kore_webgpu_cull_mode;

typedef struct kore_webgpu_primitive_state {
	kore_webgpu_primitive_topology topology;
	kore_gpu_index_format          strip_index_format;
	kore_webgpu_front_face         front_face;
	kore_webgpu_cull_mode          cull_mode;
	bool                           unclipped_depth;
} kore_webgpu_primitive_state;

typedef enum kore_webgpu_stencil_operation {
	KORE_WEBGPU_STENCIL_OPERATION_KEEP,
	KORE_WEBGPU_STENCIL_OPERATION_ZERO,
	KORE_WEBGPU_STENCIL_OPERATION_REPLACE,
	KORE_WEBGPU_STENCIL_OPERATION_INVERT,
	KORE_WEBGPU_STENCIL_OPERATION_INCREMENT_CLAMP,
	KORE_WEBGPU_STENCIL_OPERATION_DECREMENT_CLAMP,
	KORE_WEBGPU_STENCIL_OPERATION_INCREMENT_WRAP,
	KORE_WEBGPU_STENCIL_OPERATION_DECREMENT_WRAP
} kore_webgpu_stencil_operation;

typedef struct kore_webgpu_stencil_face_state {
	kore_gpu_compare_function     compare;
	kore_webgpu_stencil_operation fail_op;
	kore_webgpu_stencil_operation depth_fail_op;
	kore_webgpu_stencil_operation pass_op;
} kore_webgpu_stencil_face_state;

typedef struct kore_webgpu_depth_stencil_state {
	kore_gpu_texture_format        format;
	bool                           depth_write_enabled;
	kore_gpu_compare_function      depth_compare;
	kore_webgpu_stencil_face_state stencil_front;
	kore_webgpu_stencil_face_state stencil_back;
	uint32_t                       stencil_read_mask;
	uint32_t                       stencil_write_mask;
	int32_t                        depth_bias;
	float                          depth_bias_slope_scale;
	float                          depth_bias_clamp;
} kore_webgpu_depth_stencil_state;

typedef struct kore_webgpu_multisample_state {
	uint32_t count;
	uint32_t mask;
	bool     alpha_to_coverage_enabled;
} kore_webgpu_multisample_state;

typedef enum kore_webgpu_blend_operation {
	KORE_WEBGPU_BLEND_OPERATION_ADD,
	KORE_WEBGPU_BLEND_OPERATION_SUBTRACT,
	KORE_WEBGPU_BLEND_OPERATION_REVERSE_SUBTRACT,
	KORE_WEBGPU_BLEND_OPERATION_MIN,
	KORE_WEBGPU_BLEND_OPERATION_MAX
} kore_webgpu_blend_operation;

typedef enum kore_webgpu_blend_factor {
	KORE_WEBGPU_BLEND_FACTOR_ZERO,
	KORE_WEBGPU_BLEND_FACTOR_ONE,
	KORE_WEBGPU_BLEND_FACTOR_SRC,
	KORE_WEBGPU_BLEND_FACTOR_ONE_MINUS_SRC,
	KORE_WEBGPU_BLEND_FACTOR_SRC_ALPHA,
	KORE_WEBGPU_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	KORE_WEBGPU_BLEND_FACTOR_DST,
	KORE_WEBGPU_BLEND_FACTOR_ONE_MINUS_DST,
	KORE_WEBGPU_BLEND_FACTOR_DST_ALPHA,
	KORE_WEBGPU_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
	KORE_WEBGPU_BLEND_FACTOR_SRC_ALPHA_SATURATED,
	KORE_WEBGPU_BLEND_FACTOR_CONSTANT,
	KORE_WEBGPU_BLEND_FACTOR_ONE_MINUS_CONSTANT
} kore_webgpu_blend_factor;

typedef struct kore_webgpu_blend_component {
	kore_webgpu_blend_operation operation;
	kore_webgpu_blend_factor    src_factor;
	kore_webgpu_blend_factor    dst_factor;
} kore_webgpu_blend_component;

typedef struct kore_webgpu_blend_state {
	kore_webgpu_blend_component color;
	kore_webgpu_blend_component alpha;
} kore_webgpu_blend_state;

typedef enum kore_webgpu_color_write_flags {
	KORE_WEBGPU_COLOR_WRITE_FLAGS_RED   = 0x1,
	KORE_WEBGPU_COLOR_WRITE_FLAGS_GREEN = 0x2,
	KORE_WEBGPU_COLOR_WRITE_FLAGS_BLUE  = 0x4,
	KORE_WEBGPU_COLOR_WRITE_FLAGS_ALPHA = 0x8,
	KORE_WEBGPU_COLOR_WRITE_FLAGS_ALL   = 0xF
} kore_webgpu_color_write_flags;

typedef struct kore_webgpu_color_target_state {
	kore_gpu_texture_format format;
	kore_webgpu_blend_state blend;
	uint32_t                write_mask;
} kore_webgpu_color_target_state;

#define KORE_WEBGPU_MAX_COLOR_TARGETS 8

typedef struct kore_webgpu_fragment_state {
	kore_webgpu_shader             shader;
	kore_webgpu_color_target_state targets[KORE_WEBGPU_MAX_COLOR_TARGETS];
	size_t                         targets_count;
} kore_webgpu_fragment_state;

typedef struct kore_webgpu_render_pipeline_parameters {
	kore_webgpu_vertex_state        vertex;
	kore_webgpu_primitive_state     primitive;
	kore_webgpu_depth_stencil_state depth_stencil;
	kore_webgpu_multisample_state   multisample;
	kore_webgpu_fragment_state      fragment;
} kore_webgpu_render_pipeline_parameters;

typedef struct kore_webgpu_render_pipeline {
	WGPURenderPipeline render_pipeline;
} kore_webgpu_render_pipeline;

typedef struct kore_webgpu_compute_pipeline_parameters {
	kore_webgpu_shader shader;
} kore_webgpu_compute_pipeline_parameters;

typedef struct kore_webgpu_compute_pipeline {
	WGPUComputePipeline compute_pipeline;
} kore_webgpu_compute_pipeline;

typedef struct kore_webgpu_ray_pipeline_parameters {
	kore_webgpu_shader gen_shader;
	kore_webgpu_shader miss_shader;
	kore_webgpu_shader closest_shader;
	kore_webgpu_shader intersection_shader;
	kore_webgpu_shader any_shader;
} kore_webgpu_ray_pipeline_parameters;

typedef struct kore_webgpu_ray_pipeline {
	int nothing;
} kore_webgpu_ray_pipeline;

#ifdef __cplusplus
}
#endif

#endif
