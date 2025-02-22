#include <kore3/gpu/texture.h>

#ifdef KORE_DIRECT3D12
#include <kope/direct3d12/texture_functions.h>
#endif

#ifdef KORE_METAL
#include <kope/metal/texture_functions.h>
#endif

#ifdef KORE_VULKAN
#include <kope/vulkan/texture_functions.h>
#endif

void kore_gpu_texture_set_name(kore_gpu_texture *texture, const char *name) {
	KORE_GPU_CALL2(texture_set_name, texture, name);
}
