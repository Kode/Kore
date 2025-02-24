#include <kore3/gpu/texture.h>

#ifdef KORE_DIRECT3D12
#include <kore3/direct3d12/texture_functions.h>
#endif

#ifdef KORE_METAL
#include <kore3/metal/texture_functions.h>
#endif

#ifdef KORE_VULKAN
#include <kore3/vulkan/texture_functions.h>
#endif

#ifdef KORE_WEBGPU
#include <kore3/webgpu/texture_functions.h>
#endif

void kore_gpu_texture_set_name(kore_gpu_texture *texture, const char *name) {
	KORE_GPU_CALL2(texture_set_name, texture, name);
}
