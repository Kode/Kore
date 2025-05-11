#include <kore3/gpu/texture.h>

#if defined(KORE_DIRECT3D11)
#include <kore3/direct3d11/texture_functions.h>
#elif defined(KORE_DIRECT3D12)
#include <kore3/direct3d12/texture_functions.h>
#elif defined(KORE_METAL)
#include <kore3/metal/texture_functions.h>
#elif defined(KORE_OPENGL)
#include <kore3/opengl/texture_functions.h>
#elif defined(KORE_VULKAN)
#include <kore3/vulkan/texture_functions.h>
#elif defined(KORE_WEBGPU)
#include <kore3/webgpu/texture_functions.h>
#else
#error("Unknown GPU backend")
#endif

void kore_gpu_texture_set_name(kore_gpu_texture *texture, const char *name) {
	KORE_GPU_CALL2(texture_set_name, texture, name);
}

void kore_gpu_texture_destroy(kore_gpu_texture *texture) {
	KORE_GPU_CALL1(texture_destroy, texture);
}
