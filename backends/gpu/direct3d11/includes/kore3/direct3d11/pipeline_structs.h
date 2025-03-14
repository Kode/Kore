#ifndef KORE_D3D11_PIPELINE_STRUCTS_HEADER
#define KORE_D3D11_PIPELINE_STRUCTS_HEADER

#include <kore3/gpu/buffer.h>
#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/device.h>
#include <kore3/gpu/texture.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum kore_d3d11_vertex_step_mode { KORE_D3D11_VERTEX_STEP_MODE_VERTEX, KORE_D3D11_VERTEX_STEP_MODE_INSTANCE } kore_d3d11_vertex_step_mode;

typedef enum kore_d3d11_vertex_format {
	KORE_D3D11_VERTEX_FORMAT_UINT8X2,
	KORE_D3D11_VERTEX_FORMAT_UINT8X4,
	KORE_D3D11_VERTEX_FORMAT_SINT8X2,
	KORE_D3D11_VERTEX_FORMAT_SINT8X4,
	KORE_D3D11_VERTEX_FORMAT_UNORM8X2,
	KORE_D3D11_VERTEX_FORMAT_UNORM8X4,
	KORE_D3D11_VERTEX_FORMAT_SNORM8X2,
	KORE_D3D11_VERTEX_FORMAT_SNORM8X4,
	KORE_D3D11_VERTEX_FORMAT_UINT16X2,
	KORE_D3D11_VERTEX_FORMAT_UINT16X4,
	KORE_D3D11_VERTEX_FORMAT_SINT16X2,
	KORE_D3D11_VERTEX_FORMAT_SINT16X4,
	KORE_D3D11_VERTEX_FORMAT_UNORM16X2,
	KORE_D3D11_VERTEX_FORMAT_UNORM16X4,
	KORE_D3D11_VERTEX_FORMAT_SNORM16X2,
	KORE_D3D11_VERTEX_FORMAT_SNORM16X4,
	KORE_D3D11_VERTEX_FORMAT_FLOAT16X2,
	KORE_D3D11_VERTEX_FORMAT_FLOAT16X4,
	KORE_D3D11_VERTEX_FORMAT_FLOAT32,
	KORE_D3D11_VERTEX_FORMAT_FLOAT32X2,
	KORE_D3D11_VERTEX_FORMAT_FLOAT32X3,
	KORE_D3D11_VERTEX_FORMAT_FLOAT32X4,
	KORE_D3D11_VERTEX_FORMAT_UINT32,
	KORE_D3D11_VERTEX_FORMAT_UINT32X2,
	KORE_D3D11_VERTEX_FORMAT_UINT32X3,
	KORE_D3D11_VERTEX_FORMAT_UINT32X4,
	KORE_D3D11_VERTEX_FORMAT_SIN32,
	KORE_D3D11_VERTEX_FORMAT_SINT32X2,
	KORE_D3D11_VERTEX_FORMAT_SINT32X3,
	KORE_D3D11_VERTEX_FORMAT_SINT32X4,
	KORE_D3D11_VERTEX_FORMAT_UNORM10_10_10_2
} kore_d3d11_vertex_format;

typedef struct kore_d3d11_vertex_attribute {
	kore_d3d11_vertex_format format;
	uint64_t                 offset;
	uint32_t                 shader_location;
} kore_d3d11_vertex_attribute;

#define KORE_D3D11_MAX_VERTEX_ATTRIBUTES 32

typedef struct kore_d3d11_vertex_buffer_layout {
	uint64_t                    array_stride;
	kore_d3d11_vertex_step_mode step_mode;
	kore_d3d11_vertex_attribute attributes[KORE_D3D11_MAX_VERTEX_ATTRIBUTES];
	size_t                      attributes_count;
} kore_d3d11_vertex_buffer_layout;

typedef struct kore_d3d11_shader {
	uint8_t *data;
	size_t   size;
} kore_d3d11_shader;

#define KORE_D3D11_MAX_VERTEX_BUFFERS 16

typedef struct kore_d3d11_vertex_state {
	kore_d3d11_shader               shader;
	kore_d3d11_vertex_buffer_layout buffers[KORE_D3D11_MAX_VERTEX_BUFFERS];
	size_t                          buffers_count;
} kore_d3d11_vertex_state;

typedef enum kore_d3d11_primitive_topology {
	KORE_D3D11_PRIMITIVE_TOPOLOGY_POINT_LIST,
	KORE_D3D11_PRIMITIVE_TOPOLOGY_LINE_LIST,
	KORE_D3D11_PRIMITIVE_TOPOLOGY_LINE_STRIP,
	KORE_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	KORE_D3D11_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
} kore_d3d11_primitive_topology;

typedef enum kore_d3d11_front_face { KORE_D3D11_FRONT_FACE_CCW, KORE_D3D11_FRONT_FACE_CW } kore_d3d11_front_face;

typedef enum kore_d3d11_cull_mode { KORE_D3D11_CULL_MODE_NONE, KORE_D3D11_CULL_MODE_FRONT, KORE_D3D11_CULL_MODE_BACK } kore_d3d11_cull_mode;

typedef struct kore_d3d11_primitive_state {
	kore_d3d11_primitive_topology topology;
	kore_gpu_index_format         strip_index_format;
	kore_d3d11_front_face         front_face;
	kore_d3d11_cull_mode          cull_mode;
	bool                          unclipped_depth;
} kore_d3d11_primitive_state;

typedef enum kore_d3d11_stencil_operation {
	KORE_D3D11_STENCIL_OPERATION_KEEP,
	KORE_D3D11_STENCIL_OPERATION_ZERO,
	KORE_D3D11_STENCIL_OPERATION_REPLACE,
	KORE_D3D11_STENCIL_OPERATION_INVERT,
	KORE_D3D11_STENCIL_OPERATION_INCREMENT_CLAMP,
	KORE_D3D11_STENCIL_OPERATION_DECREMENT_CLAMP,
	KORE_D3D11_STENCIL_OPERATION_INCREMENT_WRAP,
	KORE_D3D11_STENCIL_OPERATION_DECREMENT_WRAP
} kore_d3d11_stencil_operation;

typedef struct kore_d3d11_stencil_face_state {
	kore_gpu_compare_function    compare;
	kore_d3d11_stencil_operation fail_op;
	kore_d3d11_stencil_operation depth_fail_op;
	kore_d3d11_stencil_operation pass_op;
} kore_d3d11_stencil_face_state;

typedef struct kore_d3d11_depth_stencil_state {
	kore_gpu_texture_format       format;
	bool                          depth_write_enabled;
	kore_gpu_compare_function     depth_compare;
	kore_d3d11_stencil_face_state stencil_front;
	kore_d3d11_stencil_face_state stencil_back;
	uint32_t                      stencil_read_mask;
	uint32_t                      stencil_write_mask;
	int32_t                       depth_bias;
	float                         depth_bias_slope_scale;
	float                         depth_bias_clamp;
} kore_d3d11_depth_stencil_state;

typedef struct kore_d3d11_multisample_state {
	uint32_t count;
	uint32_t mask;
	bool     alpha_to_coverage_enabled;
} kore_d3d11_multisample_state;

typedef enum kore_d3d11_blend_operation {
	KORE_D3D11_BLEND_OPERATION_ADD,
	KORE_D3D11_BLEND_OPERATION_SUBTRACT,
	KORE_D3D11_BLEND_OPERATION_REVERSE_SUBTRACT,
	KORE_D3D11_BLEND_OPERATION_MIN,
	KORE_D3D11_BLEND_OPERATION_MAX
} kore_d3d11_blend_operation;

typedef enum kore_d3d11_blend_factor {
	KORE_D3D11_BLEND_FACTOR_ZERO,
	KORE_D3D11_BLEND_FACTOR_ONE,
	KORE_D3D11_BLEND_FACTOR_SRC,
	KORE_D3D11_BLEND_FACTOR_ONE_MINUS_SRC,
	KORE_D3D11_BLEND_FACTOR_SRC_ALPHA,
	KORE_D3D11_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	KORE_D3D11_BLEND_FACTOR_DST,
	KORE_D3D11_BLEND_FACTOR_ONE_MINUS_DST,
	KORE_D3D11_BLEND_FACTOR_DST_ALPHA,
	KORE_D3D11_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
	KORE_D3D11_BLEND_FACTOR_SRC_ALPHA_SATURATED,
	KORE_D3D11_BLEND_FACTOR_CONSTANT,
	KORE_D3D11_BLEND_FACTOR_ONE_MINUS_CONSTANT
} kore_d3d11_blend_factor;

typedef struct kore_d3d11_blend_component {
	kore_d3d11_blend_operation operation;
	kore_d3d11_blend_factor    src_factor;
	kore_d3d11_blend_factor    dst_factor;
} kore_d3d11_blend_component;

typedef struct kore_d3d11_blend_state {
	kore_d3d11_blend_component color;
	kore_d3d11_blend_component alpha;
} kore_d3d11_blend_state;

typedef enum kore_d3d11_color_write_flags {
	KORE_D3D11_COLOR_WRITE_FLAGS_RED   = 0x1,
	KORE_D3D11_COLOR_WRITE_FLAGS_GREEN = 0x2,
	KORE_D3D11_COLOR_WRITE_FLAGS_BLUE  = 0x4,
	KORE_D3D11_COLOR_WRITE_FLAGS_ALPHA = 0x8,
	KORE_D3D11_COLOR_WRITE_FLAGS_ALL   = 0xF
} kore_d3d11_color_write_flags;

typedef struct kore_d3d11_color_target_state {
	kore_gpu_texture_format format;
	kore_d3d11_blend_state  blend;
	uint32_t                write_mask;
} kore_d3d11_color_target_state;

#define KORE_D3D11_MAX_COLOR_TARGETS 8

typedef struct kore_d3d11_fragment_state {
	kore_d3d11_shader             shader;
	kore_d3d11_color_target_state targets[KORE_D3D11_MAX_COLOR_TARGETS];
	size_t                        targets_count;
} kore_d3d11_fragment_state;

typedef struct kore_d3d11_render_pipeline_parameters {
	kore_d3d11_vertex_state        vertex;
	kore_d3d11_primitive_state     primitive;
	kore_d3d11_depth_stencil_state depth_stencil;
	kore_d3d11_multisample_state   multisample;
	kore_d3d11_fragment_state      fragment;
} kore_d3d11_render_pipeline_parameters;

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11DepthStencilState;
struct ID3D11RasterizerState;

typedef struct kore_d3d11_render_pipeline {
	struct ID3D11VertexShader      *vertex_shader;
	struct ID3D11PixelShader       *fragment_shader;
	struct ID3D11InputLayout       *input_layout;
	struct ID3D11DepthStencilState *depth_stencil_state;
	struct ID3D11RasterizerState   *rasterizer_state;
} kore_d3d11_render_pipeline;

typedef struct kore_d3d11_compute_pipeline_parameters {
	kore_d3d11_shader shader;
} kore_d3d11_compute_pipeline_parameters;

typedef struct kore_d3d11_compute_pipeline {
	int nothing;
} kore_d3d11_compute_pipeline;

typedef struct kore_d3d11_ray_pipeline_parameters {
	kore_d3d11_shader gen_shader;
	kore_d3d11_shader miss_shader;
	kore_d3d11_shader closest_shader;
	kore_d3d11_shader intersection_shader;
	kore_d3d11_shader any_shader;
} kore_d3d11_ray_pipeline_parameters;

typedef struct kore_d3d11_ray_pipeline {
	int nothing;
} kore_d3d11_ray_pipeline;

#ifdef __cplusplus
}
#endif

#endif
