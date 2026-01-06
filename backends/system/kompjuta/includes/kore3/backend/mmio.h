#ifndef KOMPJUTA_MMIO_HEADER
#define KOMPJUTA_MMIO_HEADER

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
	} data;
} kompjuta_gpu_command;

#ifdef __cplusplus
}
#endif

#endif
