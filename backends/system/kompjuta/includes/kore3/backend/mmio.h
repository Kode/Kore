#ifndef KOMPJUTA_MMIO_HEADER
#define KOMPJUTA_MMIO_HEADER

#include <kore3/gpu/commandlist.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MMIO_BASE 0xffffffff00000000

#define FB_ADDR              0x0
#define FB_STRIDE            0x08
#define FB_WIDTH             0x0c
#define FB_HEIGHT            0x10
#define FB_FORMAT            0x14
#define PRESENT              0x18
#define COMMAND_LIST_ADDR    0x20
#define COMMAND_LIST_SIZE    0x28
#define EXECUTE_COMMAND_LIST 0x32

typedef enum kompjuta_gpu_command_kind {
	KOMPJUTA_GPU_COMMAND_CLEAR,
	KOMPJUTA_GPU_COMMAND_SET_INDEX_BUFFER,
	KOMPJUTA_GPU_COMMAND_SET_VERTEX_BUFFER,
	KOMPJUTA_GPU_COMMAND_SET_RENDER_PIPELINE,
	KOMPJUTA_GPU_COMMAND_DRAW_INDEXED,
	KOMPJUTA_GPU_COMMAND_PRESENT,
} kompjuta_gpu_command_kind;

typedef struct kompjuta_gpu_command {
	kompjuta_gpu_command_kind kind;
	union {
		struct {
			float r;
			float g;
			float b;
			float a;
		} clear;
		struct {
			void                 *data;
			kore_gpu_index_format index_format;
		} set_index_buffer;
		struct {
			void    *data;
			uint64_t stride;
		} set_vertex_buffer;
		struct {
			void *vertex_shader;
			void *fragment_shader;
		} set_render_pipeline;
		struct {
			uint32_t index_count;
			uint32_t instance_count;
			uint32_t first_index;
			int32_t  base_vertex;
			uint32_t first_instance;
		} draw_indexed;
	} data;
} kompjuta_gpu_command;

#ifdef __cplusplus
}
#endif

#endif
