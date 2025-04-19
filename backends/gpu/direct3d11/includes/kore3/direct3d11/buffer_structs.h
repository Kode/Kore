#ifndef KORE_D3D11_BUFFER_STRUCTS_HEADER
#define KORE_D3D11_BUFFER_STRUCTS_HEADER

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct kore_gpu_device;

struct ID3D11DeviceContext;
struct ID3D11Buffer;

// add-ons to kore_gpu_buffer_usage, top 16 bits only
typedef enum kore_d3d11_buffer_usage {
	KORE_D3D11_BUFFER_USAGE_VERTEX = 0x00010000,
	KORE_D3D11_BUFFER_USAGE_CBV    = 0x00020000,
	KORE_D3D11_BUFFER_USAGE_SRV    = 0x00040000,
	KORE_D3D11_BUFFER_USAGE_UAV    = 0x00080000,
} kore_d3d11_buffer_usage;

typedef struct kore_d3d11_buffer {
	struct ID3D11DeviceContext *context;
	struct ID3D11Buffer        *buffer;
} kore_d3d11_buffer;

#ifdef __cplusplus
}
#endif

#endif
