#ifndef KORE_KOMPJUTA_COMMANDLIST_STRUCTS_HEADER
#define KORE_KOMPJUTA_COMMANDLIST_STRUCTS_HEADER

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <kore3/backend/mmio.h>

typedef struct kore_kompjuta_command_list {
	kompjuta_gpu_command *commands;
	uint32_t              commands_count;
	uint32_t              current_command;
	void                 *shader_stack;
	uint32_t              shader_stack_size;
} kore_kompjuta_command_list;

#ifdef __cplusplus
}
#endif

#endif
