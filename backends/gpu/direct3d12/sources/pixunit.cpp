#include "pixunit.h"

#ifdef KORE_PIX
#include "d3d12unit.h"

#include <WinPixEventRuntime/pix3.h>

void kore_pix_push_debug_group(struct ID3D12GraphicsCommandList2 *list, const char *name) {
	PIXBeginEvent(list, 0, "%s", name);
}

void kore_pix_pop_debug_group(struct ID3D12GraphicsCommandList2 *list) {
	PIXEndEvent(list);
}

void kore_pix_insert_debug_marker(struct ID3D12GraphicsCommandList2 *list, const char *name) {
	PIXSetMarker(list, 0, "%s", name);
}

#endif
