#include <kore3/util/indexallocator.h>

void kore_index_allocator_init(kore_index_allocator *allocator) {
	for (uint32_t i = 0; i < KORE_INDEX_ALLOCATOR_SIZE; ++i) {
		allocator->indices[i] = i;
	}
	allocator->next_indices_index = 0;
	allocator->remaining          = KORE_INDEX_ALLOCATOR_SIZE;
}

uint32_t kore_index_allocator_allocate(kore_index_allocator *allocator) {
	if (allocator->remaining == 0) {
		return 0xffffffff;
	}

	uint32_t index = allocator->indices[allocator->next_indices_index];

	allocator->next_indices_index += 1;
	if (allocator->next_indices_index >= KORE_INDEX_ALLOCATOR_SIZE) {
		allocator->next_indices_index = 0;
	}

	allocator->remaining -= 1;

	return index;
}

void kore_index_allocator_free(kore_index_allocator *allocator, uint32_t index) {
	uint32_t indices_index = allocator->next_indices_index + allocator->remaining;
	if (indices_index >= KORE_INDEX_ALLOCATOR_SIZE) {
		indices_index -= KORE_INDEX_ALLOCATOR_SIZE;
	}

	allocator->indices[indices_index] = index;

	allocator->remaining += 1;
}
