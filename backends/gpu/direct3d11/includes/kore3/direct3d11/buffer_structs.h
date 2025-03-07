#ifndef KORE_D3D11_BUFFER_STRUCTS_HEADER
#define KORE_D3D11_BUFFER_STRUCTS_HEADER

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

struct ID3D11DeviceContext;
struct ID3D11Buffer;

typedef struct kore_d3d11_buffer {
	struct ID3D11DeviceContext *context;
	struct ID3D11Buffer *buffer;
} kore_d3d11_buffer;

#ifdef __cplusplus
}
#endif

#endif
