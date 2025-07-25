#ifndef KORE_VULKAN_PIPELINE_STRUCTS_HEADER
#define KORE_VULKAN_PIPELINE_STRUCTS_HEADER

#include <kore3/gpu/buffer.h>
#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/device.h>
#include <kore3/gpu/texture.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum kore_vulkan_vertex_step_mode { KORE_VULKAN_VERTEX_STEP_MODE_VERTEX, KORE_VULKAN_VERTEX_STEP_MODE_INSTANCE } kore_vulkan_vertex_step_mode;

typedef enum kore_vulkan_vertex_format {
	KORE_VULKAN_VERTEX_FORMAT_UINT8X2,
	KORE_VULKAN_VERTEX_FORMAT_UINT8X4,
	KORE_VULKAN_VERTEX_FORMAT_SINT8X2,
	KORE_VULKAN_VERTEX_FORMAT_SINT8X4,
	KORE_VULKAN_VERTEX_FORMAT_UNORM8X2,
	KORE_VULKAN_VERTEX_FORMAT_UNORM8X4,
	KORE_VULKAN_VERTEX_FORMAT_SNORM8X2,
	KORE_VULKAN_VERTEX_FORMAT_SNORM8X4,
	KORE_VULKAN_VERTEX_FORMAT_UINT16X2,
	KORE_VULKAN_VERTEX_FORMAT_UINT16X4,
	KORE_VULKAN_VERTEX_FORMAT_SINT16X2,
	KORE_VULKAN_VERTEX_FORMAT_SINT16X4,
	KORE_VULKAN_VERTEX_FORMAT_UNORM16X2,
	KORE_VULKAN_VERTEX_FORMAT_UNORM16X4,
	KORE_VULKAN_VERTEX_FORMAT_SNORM16X2,
	KORE_VULKAN_VERTEX_FORMAT_SNORM16X4,
	KORE_VULKAN_VERTEX_FORMAT_FLOAT16X2,
	KORE_VULKAN_VERTEX_FORMAT_FLOAT16X4,
	KORE_VULKAN_VERTEX_FORMAT_FLOAT32,
	KORE_VULKAN_VERTEX_FORMAT_FLOAT32X2,
	KORE_VULKAN_VERTEX_FORMAT_FLOAT32X3,
	KORE_VULKAN_VERTEX_FORMAT_FLOAT32X4,
	KORE_VULKAN_VERTEX_FORMAT_UINT32,
	KORE_VULKAN_VERTEX_FORMAT_UINT32X2,
	KORE_VULKAN_VERTEX_FORMAT_UINT32X3,
	KORE_VULKAN_VERTEX_FORMAT_UINT32X4,
	KORE_VULKAN_VERTEX_FORMAT_SINT32,
	KORE_VULKAN_VERTEX_FORMAT_SINT32X2,
	KORE_VULKAN_VERTEX_FORMAT_SINT32X3,
	KORE_VULKAN_VERTEX_FORMAT_SINT32X4,
	KORE_VULKAN_VERTEX_FORMAT_UNORM10_10_10_2
} kore_vulkan_vertex_format;

typedef struct kore_vulkan_vertex_attribute {
	kore_vulkan_vertex_format format;
	uint64_t                  offset;
	uint32_t                  shader_location;
} kore_vulkan_vertex_attribute;

#define KORE_VULKAN_MAX_VERTEX_ATTRIBUTES 32

typedef struct kore_vulkan_vertex_buffer_layout {
	uint64_t                     array_stride;
	kore_vulkan_vertex_step_mode step_mode;
	kore_vulkan_vertex_attribute attributes[KORE_VULKAN_MAX_VERTEX_ATTRIBUTES];
	size_t                       attributes_count;
} kore_vulkan_vertex_buffer_layout;

typedef struct kore_vulkan_shader {
	uint8_t *data;
	size_t   size;
} kore_vulkan_shader;

#define KORE_VULKAN_MAX_VERTEX_BUFFERS 16

typedef struct kore_vulkan_vertex_state {
	kore_vulkan_shader               shader;
	kore_vulkan_vertex_buffer_layout buffers[KORE_VULKAN_MAX_VERTEX_BUFFERS];
	size_t                           buffers_count;
} kore_vulkan_vertex_state;

typedef enum kore_vulkan_primitive_topology {
	KORE_VULKAN_PRIMITIVE_TOPOLOGY_POINT_LIST,
	KORE_VULKAN_PRIMITIVE_TOPOLOGY_LINE_LIST,
	KORE_VULKAN_PRIMITIVE_TOPOLOGY_LINE_STRIP,
	KORE_VULKAN_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	KORE_VULKAN_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
} kore_vulkan_primitive_topology;

typedef enum kore_vulkan_front_face { KORE_VULKAN_FRONT_FACE_CCW, KORE_VULKAN_FRONT_FACE_CW } kore_vulkan_front_face;

typedef enum kore_vulkan_cull_mode { KORE_VULKAN_CULL_MODE_NONE, KORE_VULKAN_CULL_MODE_FRONT, KORE_VULKAN_CULL_MODE_BACK } kore_vulkan_cull_mode;

typedef struct kore_vulkan_primitive_state {
	kore_vulkan_primitive_topology topology;
	kore_gpu_index_format          strip_index_format;
	kore_vulkan_front_face         front_face;
	kore_vulkan_cull_mode          cull_mode;
	bool                           unclipped_depth;
} kore_vulkan_primitive_state;

typedef enum kore_vulkan_stencil_operation {
	KORE_VULKAN_STENCIL_OPERATION_KEEP,
	KORE_VULKAN_STENCIL_OPERATION_ZERO,
	KORE_VULKAN_STENCIL_OPERATION_REPLACE,
	KORE_VULKAN_STENCIL_OPERATION_INVERT,
	KORE_VULKAN_STENCIL_OPERATION_INCREMENT_CLAMP,
	KORE_VULKAN_STENCIL_OPERATION_DECREMENT_CLAMP,
	KORE_VULKAN_STENCIL_OPERATION_INCREMENT_WRAP,
	KORE_VULKAN_STENCIL_OPERATION_DECREMENT_WRAP
} kore_vulkan_stencil_operation;

typedef struct kore_vulkan_stencil_face_state {
	kore_gpu_compare_function     compare;
	kore_vulkan_stencil_operation fail_op;
	kore_vulkan_stencil_operation depth_fail_op;
	kore_vulkan_stencil_operation pass_op;
} kore_vulkan_stencil_face_state;

typedef struct kore_vulkan_depth_stencil_state {
	kore_gpu_texture_format        format;
	bool                           depth_write_enabled;
	kore_gpu_compare_function      depth_compare;
	kore_vulkan_stencil_face_state stencil_front;
	kore_vulkan_stencil_face_state stencil_back;
	uint32_t                       stencil_read_mask;
	uint32_t                       stencil_write_mask;
	int32_t                        depth_bias;
	float                          depth_bias_slope_scale;
	float                          depth_bias_clamp;
} kore_vulkan_depth_stencil_state;

typedef struct kore_vulkan_multisample_state {
	uint32_t count;
	uint32_t mask;
	bool     alpha_to_coverage_enabled;
} kore_vulkan_multisample_state;

typedef enum kore_vulkan_blend_operation {
	KORE_VULKAN_BLEND_OPERATION_ADD,
	KORE_VULKAN_BLEND_OPERATION_SUBTRACT,
	KORE_VULKAN_BLEND_OPERATION_REVERSE_SUBTRACT,
	KORE_VULKAN_BLEND_OPERATION_MIN,
	KORE_VULKAN_BLEND_OPERATION_MAX
} kore_vulkan_blend_operation;

typedef enum kore_vulkan_blend_factor {
	KORE_VULKAN_BLEND_FACTOR_ZERO,
	KORE_VULKAN_BLEND_FACTOR_ONE,
	KORE_VULKAN_BLEND_FACTOR_SRC,
	KORE_VULKAN_BLEND_FACTOR_ONE_MINUS_SRC,
	KORE_VULKAN_BLEND_FACTOR_SRC_ALPHA,
	KORE_VULKAN_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	KORE_VULKAN_BLEND_FACTOR_DST,
	KORE_VULKAN_BLEND_FACTOR_ONE_MINUS_DST,
	KORE_VULKAN_BLEND_FACTOR_DST_ALPHA,
	KORE_VULKAN_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
	KORE_VULKAN_BLEND_FACTOR_SRC_ALPHA_SATURATED,
	KORE_VULKAN_BLEND_FACTOR_CONSTANT,
	KORE_VULKAN_BLEND_FACTOR_ONE_MINUS_CONSTANT
} kore_vulkan_blend_factor;

typedef struct kore_vulkan_blend_component {
	kore_vulkan_blend_operation operation;
	kore_vulkan_blend_factor    src_factor;
	kore_vulkan_blend_factor    dst_factor;
} kore_vulkan_blend_component;

typedef struct kore_vulkan_blend_state {
	kore_vulkan_blend_component color;
	kore_vulkan_blend_component alpha;
} kore_vulkan_blend_state;

typedef enum kore_vulkan_color_write_flags {
	KORE_VULKAN_COLOR_WRITE_FLAGS_RED   = 0x1,
	KORE_VULKAN_COLOR_WRITE_FLAGS_GREEN = 0x2,
	KORE_VULKAN_COLOR_WRITE_FLAGS_BLUE  = 0x4,
	KORE_VULKAN_COLOR_WRITE_FLAGS_ALPHA = 0x8,
	KORE_VULKAN_COLOR_WRITE_FLAGS_ALL   = 0xF
} kore_vulkan_color_write_flags;

typedef struct kore_vulkan_color_target_state {
	kore_gpu_texture_format format;
	kore_vulkan_blend_state blend;
	uint32_t                write_mask;
} kore_vulkan_color_target_state;

#define KORE_VULKAN_MAX_COLOR_TARGETS 8

typedef struct kore_vulkan_fragment_state {
	kore_vulkan_shader             shader;
	kore_vulkan_color_target_state targets[KORE_VULKAN_MAX_COLOR_TARGETS];
	size_t                         targets_count;
} kore_vulkan_fragment_state;

typedef struct kore_vulkan_render_pipeline_parameters {
	kore_vulkan_vertex_state        vertex;
	kore_vulkan_primitive_state     primitive;
	kore_vulkan_depth_stencil_state depth_stencil;
	kore_vulkan_multisample_state   multisample;
	kore_vulkan_fragment_state      fragment;
} kore_vulkan_render_pipeline_parameters;

typedef struct kore_vulkan_render_pipeline {
	VkPipelineLayout pipeline_layout;
	VkPipeline       pipeline;
	VkRenderPass     render_pass;
} kore_vulkan_render_pipeline;

typedef struct kore_vulkan_compute_pipeline_parameters {
	kore_vulkan_shader shader;
} kore_vulkan_compute_pipeline_parameters;

typedef struct kore_vulkan_compute_pipeline {
	VkPipelineLayout pipeline_layout;
	VkPipeline       pipeline;
} kore_vulkan_compute_pipeline;

typedef struct kore_vulkan_ray_pipeline_parameters {
	kore_vulkan_shader gen_shader;
	kore_vulkan_shader miss_shader;
	kore_vulkan_shader closest_shader;
	kore_vulkan_shader intersection_shader;
	kore_vulkan_shader any_shader;
} kore_vulkan_ray_pipeline_parameters;

typedef struct kore_vulkan_ray_pipeline {
	int nothing;
} kore_vulkan_ray_pipeline;

#ifdef __cplusplus
}
#endif

#endif
