#ifndef KORE_PIX_UNIT_HEADER
#define KORE_PIX_UNIT_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#ifdef KORE_PIX
void kore_pix_push_debug_group(struct ID3D12GraphicsCommandList2 *list, const char *name);
void kore_pix_pop_debug_group(struct ID3D12GraphicsCommandList2 *list);
void kore_pix_insert_debug_marker(struct ID3D12GraphicsCommandList2 *list, const char *name);
#endif

#ifdef __cplusplus
}
#endif

#endif
