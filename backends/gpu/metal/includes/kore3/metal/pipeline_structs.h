#ifndef KORE_METAL_PIPELINE_STRUCTS_HEADER
#define KORE_METAL_PIPELINE_STRUCTS_HEADER

#include <kore3/gpu/buffer.h>
#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/device.h>
#include <kore3/gpu/texture.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum kore_metal_vertex_step_mode { KORE_METAL_VERTEX_STEP_MODE_VERTEX, KORE_METAL_VERTEX_STEP_MODE_INSTANCE } kore_metal_vertex_step_mode;

typedef enum kore_metal_vertex_format {
	KORE_METAL_VERTEX_FORMAT_UINT8X2,
	KORE_METAL_VERTEX_FORMAT_UINT8X4,
	KORE_METAL_VERTEX_FORMAT_SINT8X2,
	KORE_METAL_VERTEX_FORMAT_SINT8X4,
	KORE_METAL_VERTEX_FORMAT_UNORM8X2,
	KORE_METAL_VERTEX_FORMAT_UNORM8X4,
	KORE_METAL_VERTEX_FORMAT_SNORM8X2,
	KORE_METAL_VERTEX_FORMAT_SNORM8X4,
	KORE_METAL_VERTEX_FORMAT_UINT16X2,
	KORE_METAL_VERTEX_FORMAT_UINT16X4,
	KORE_METAL_VERTEX_FORMAT_SINT16X2,
	KORE_METAL_VERTEX_FORMAT_SINT16X4,
	KORE_METAL_VERTEX_FORMAT_UNORM16X2,
	KORE_METAL_VERTEX_FORMAT_UNORM16X4,
	KORE_METAL_VERTEX_FORMAT_SNORM16X2,
	KORE_METAL_VERTEX_FORMAT_SNORM16X4,
	KORE_METAL_VERTEX_FORMAT_FLOAT16X2,
	KORE_METAL_VERTEX_FORMAT_FLOAT16X4,
	KORE_METAL_VERTEX_FORMAT_FLOAT32,
	KORE_METAL_VERTEX_FORMAT_FLOAT32X2,
	KORE_METAL_VERTEX_FORMAT_FLOAT32X3,
	KORE_METAL_VERTEX_FORMAT_FLOAT32X4,
	KORE_METAL_VERTEX_FORMAT_UINT32,
	KORE_METAL_VERTEX_FORMAT_UINT32X2,
	KORE_METAL_VERTEX_FORMAT_UINT32X3,
	KORE_METAL_VERTEX_FORMAT_UINT32X4,
	KORE_METAL_VERTEX_FORMAT_SINT32,
	KORE_METAL_VERTEX_FORMAT_SINT32X2,
	KORE_METAL_VERTEX_FORMAT_SINT32X3,
	KORE_METAL_VERTEX_FORMAT_SINT32X4,
	KORE_METAL_VERTEX_FORMAT_UNORM10_10_10_2
} kore_metal_vertex_format;

typedef struct kore_metal_vertex_attribute {
	kore_metal_vertex_format format;
	uint64_t offset;
	uint32_t shader_location;
} kore_metal_vertex_attribute;

#define KORE_METAL_MAX_VERTEX_ATTRIBUTES 32

typedef struct kore_metal_vertex_buffer_layout {
	uint64_t array_stride;
	kore_metal_vertex_step_mode step_mode;
	kore_metal_vertex_attribute attributes[KORE_METAL_MAX_VERTEX_ATTRIBUTES];
	size_t attributes_count;
} kore_metal_vertex_buffer_layout;

typedef struct kore_metal_shader {
	const char *function_name;
} kore_metal_shader;

#define KORE_METAL_MAX_VERTEX_BUFFERS 16

typedef struct kore_metal_vertex_state {
	kore_metal_shader shader;
	kore_metal_vertex_buffer_layout buffers[KORE_METAL_MAX_VERTEX_BUFFERS];
	size_t buffers_count;
} kore_metal_vertex_state;

typedef enum kore_metal_primitive_topology {
	KORE_METAL_PRIMITIVE_TOPOLOGY_POINT_LIST,
	KORE_METAL_PRIMITIVE_TOPOLOGY_LINE_LIST,
	KORE_METAL_PRIMITIVE_TOPOLOGY_LINE_STRIP,
	KORE_METAL_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	KORE_METAL_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
} kore_metal_primitive_topology;

typedef enum kore_metal_front_face { KORE_METAL_FRONT_FACE_CCW, KORE_METAL_FRONT_FACE_CW } kore_metal_front_face;

typedef enum kore_metal_cull_mode { KORE_METAL_CULL_MODE_NONE, KORE_METAL_CULL_MODE_FRONT, KORE_METAL_CULL_MODE_BACK } kore_metal_cull_mode;

typedef struct kore_metal_primitive_state {
	kore_metal_primitive_topology topology;
	kore_gpu_index_format strip_index_format;
	kore_metal_front_face front_face;
	kore_metal_cull_mode cull_mode;
	bool unclipped_depth;
} kore_metal_primitive_state;

typedef enum kore_metal_stencil_operation {
	KORE_METAL_STENCIL_OPERATION_KEEP,
	KORE_METAL_STENCIL_OPERATION_ZERO,
	KORE_METAL_STENCIL_OPERATION_REPLACE,
	KORE_METAL_STENCIL_OPERATION_INVERT,
	KORE_METAL_STENCIL_OPERATION_INCREMENT_CLAMP,
	KORE_METAL_STENCIL_OPERATION_DECREMENT_CLAMP,
	KORE_METAL_STENCIL_OPERATION_INCREMENT_WRAP,
	KORE_METAL_STENCIL_OPERATION_DECREMENT_WRAP
} kore_metal_stencil_operation;

typedef struct kore_metal_stencil_face_state {
	kore_gpu_compare_function compare;
	kore_metal_stencil_operation fail_op;
	kore_metal_stencil_operation depth_fail_op;
	kore_metal_stencil_operation pass_op;
} kore_metal_stencil_face_state;

typedef struct kore_metal_depth_stencil_state {
	kore_gpu_texture_format format;
	bool depth_write_enabled;
	kore_gpu_compare_function depth_compare;
	kore_metal_stencil_face_state stencil_front;
	kore_metal_stencil_face_state stencil_back;
	uint32_t stencil_read_mask;
	uint32_t stencil_write_mask;
	int32_t depth_bias;
	float depth_bias_slope_scale;
	float depth_bias_clamp;
} kore_metal_depth_stencil_state;

typedef struct kore_metal_multisample_state {
	uint32_t count;
	uint32_t mask;
	bool alpha_to_coverage_enabled;
} kore_metal_multisample_state;

typedef enum kore_metal_blend_operation {
	KORE_METAL_BLEND_OPERATION_ADD,
	KORE_METAL_BLEND_OPERATION_SUBTRACT,
	KORE_METAL_BLEND_OPERATION_REVERSE_SUBTRACT,
	KORE_METAL_BLEND_OPERATION_MIN,
	KORE_METAL_BLEND_OPERATION_MAX
} kore_metal_blend_operation;

typedef enum kore_metal_blend_factor {
	KORE_METAL_BLEND_FACTOR_ZERO,
	KORE_METAL_BLEND_FACTOR_ONE,
	KORE_METAL_BLEND_FACTOR_SRC,
	KORE_METAL_BLEND_FACTOR_ONE_MINUS_SRC,
	KORE_METAL_BLEND_FACTOR_SRC_ALPHA,
	KORE_METAL_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	KORE_METAL_BLEND_FACTOR_DST,
	KORE_METAL_BLEND_FACTOR_ONE_MINUS_DST,
	KORE_METAL_BLEND_FACTOR_DST_ALPHA,
	KORE_METAL_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
	KORE_METAL_BLEND_FACTOR_SRC_ALPHA_SATURATED,
	KORE_METAL_BLEND_FACTOR_CONSTANT,
	KORE_METAL_BLEND_FACTOR_ONE_MINUS_CONSTANT
} kore_metal_blend_factor;

typedef struct kore_metal_blend_component {
	kore_metal_blend_operation operation;
	kore_metal_blend_factor src_factor;
	kore_metal_blend_factor dst_factor;
} kore_metal_blend_component;

typedef struct kore_metal_blend_state {
	kore_metal_blend_component color;
	kore_metal_blend_component alpha;
} kore_metal_blend_state;

typedef enum kore_metal_color_write_flags {
	KORE_METAL_COLOR_WRITE_FLAGS_RED   = 0x1,
	KORE_METAL_COLOR_WRITE_FLAGS_GREEN = 0x2,
	KORE_METAL_COLOR_WRITE_FLAGS_BLUE  = 0x4,
	KORE_METAL_COLOR_WRITE_FLAGS_ALPHA = 0x8,
	KORE_METAL_COLOR_WRITE_FLAGS_ALL   = 0xF
} kore_metal_color_write_flags;

typedef struct kore_metal_color_target_state {
	kore_gpu_texture_format format;
	kore_metal_blend_state blend;
	uint32_t write_mask;
} kore_metal_color_target_state;

#define KORE_METAL_MAX_COLOR_TARGETS 8

typedef struct kore_metal_fragment_state {
	kore_metal_shader shader;
	kore_metal_color_target_state targets[KORE_METAL_MAX_COLOR_TARGETS];
	size_t targets_count;
} kore_metal_fragment_state;

typedef struct kore_metal_render_pipeline_parameters {
	kore_metal_vertex_state vertex;
	kore_metal_primitive_state primitive;
	kore_metal_depth_stencil_state depth_stencil;
	kore_metal_multisample_state multisample;
	kore_metal_fragment_state fragment;
} kore_metal_render_pipeline_parameters;

typedef struct kore_metal_render_pipeline {
	void *pipeline;
} kore_metal_render_pipeline;

typedef struct kore_metal_compute_pipeline_parameters {
	kore_metal_shader shader;
} kore_metal_compute_pipeline_parameters;

typedef struct kore_metal_compute_pipeline {
	int nothing;
} kore_metal_compute_pipeline;

typedef struct kore_metal_ray_pipeline_parameters {
	kore_metal_shader gen_shader;
	kore_metal_shader miss_shader;
	kore_metal_shader closest_shader;
	kore_metal_shader intersection_shader;
	kore_metal_shader any_shader;
} kore_metal_ray_pipeline_parameters;

typedef struct kore_metal_ray_pipeline {
	int nothing;
} kore_metal_ray_pipeline;

#ifdef __cplusplus
}
#endif

#endif
