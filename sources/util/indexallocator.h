#ifndef KORE_INDEX_ALLOCATOR_HEADER
#define KORE_INDEX_ALLOCATOR_HEADER

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KORE_INDEX_ALLOCATOR_SIZE 1024 * 10

typedef struct kore_index_allocator {
	uint32_t indices[KORE_INDEX_ALLOCATOR_SIZE];
	uint32_t next_indices_index;
	uint32_t remaining;
} kore_index_allocator;

void kore_index_allocator_init(kore_index_allocator *allocator);

uint32_t kore_index_allocator_allocate(kore_index_allocator *allocator);

void kore_index_allocator_free(kore_index_allocator *allocator, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif
