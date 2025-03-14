#ifndef KORE_D3D11_TEXTURE_STRUCTS_HEADER
#define KORE_D3D11_TEXTURE_STRUCTS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

struct ID3D11Texture2D;
struct ID3D11RenderTargetView;

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
