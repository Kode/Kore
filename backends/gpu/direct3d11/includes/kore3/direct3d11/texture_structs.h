#ifndef KORE_D3D11_TEXTURE_STRUCTS_HEADER
#define KORE_D3D11_TEXTURE_STRUCTS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

struct ID3D11Texture2D;
struct ID3D11RenderTargetView;

// add-ons to kore_gpu_texture_usage, top 16 bits only
typedef enum kore_d3d11_texture_usage {
	KORE_D3D11_TEXTURE_USAGE_SRV         = 0x00010000,
	KORE_D3D11_TEXTURE_USAGE_UAV         = 0x00020000,
	KORE_D3D11_TEXTURE_USAGE_FRAMEBUFFER = 0x00040000,
} kore_d3d11_texture_usage;

typedef struct kore_d3d11_texture {
	struct ID3D11Texture2D        *texture;
	struct ID3D11RenderTargetView *render_target_view;
	uint32_t                       width;
	uint32_t                       height;
} kore_d3d11_texture;

#ifdef __cplusplus
}
#endif

#endif
