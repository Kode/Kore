#ifndef KORE_OPENGL_COMMANDLIST_STRUCTS_HEADER
#define KORE_OPENGL_COMMANDLIST_STRUCTS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

struct kore_opengl_device;
struct kore_opengl_texture;
struct kore_opengl_compute_pipeline;
struct kore_opengl_ray_pipeline;
struct kore_opengl_rendery_pipeline;
struct kore_opengl_descriptor_set;
struct kore_gpu_query_set;

typedef struct kore_opengl_buffer_access {
	int nothing;
} kore_opengl_buffer_access;

typedef struct kore_opengl_command_list {
	uint8_t *commands;
	uint64_t commands_offset;
} kore_opengl_command_list;

#ifdef __cplusplus
}
#endif

#endif
