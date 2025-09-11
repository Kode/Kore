#ifndef KORE_D3D11_DEVICE_STRUCTS_HEADER
#define KORE_D3D11_DEVICE_STRUCTS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define KORE_D3D11_FRAME_COUNT 2

struct ID3D11Device;
struct ID3D11DeviceContext;

typedef struct kore_d3d11_device {
	struct ID3D11Device        *device;
	struct ID3D11DeviceContext *context;
} kore_d3d11_device;

typedef struct kore_d3d11_query_set {
	int nothing;
} kore_d3d11_query_set;

#ifdef __cplusplus
}
#endif

#endif
