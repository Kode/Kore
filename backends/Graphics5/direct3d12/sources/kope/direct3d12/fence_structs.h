#ifndef KORE_D3D12_FENCE_STRUCTS_HEADER
#define KORE_D3D12_FENCE_STRUCTS_HEADER

#include <kore3/gpu/buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ID3D12Fence;

typedef struct kore_d3d12_fence {
	struct ID3D12Fence *fence;
} kore_d3d12_fence;

#ifdef __cplusplus
}
#endif

#endif
