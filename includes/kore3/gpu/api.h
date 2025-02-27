#ifndef KORE_GPU_API_HEADER
#define KORE_GPU_API_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NDEBUG
#define KORE_GPU_VALIDATION
#endif

typedef enum kore_gpu_api { KORE_GPU_API_DIRECT3D12, KORE_GPU_API_VULKAN, KORE_GPU_API_METAL, KORE_GPU_API_WEBGPU } kore_gpu_api;

#if defined(KORE_DIRECT3D11)

#define KORE_GPU_IMPL(name) kore_d3d11_##name d3d11
#define KORE_GPU_CALL(name) kore_d3d11_##name()
#define KORE_GPU_CALL1(name, arg0) kore_d3d11_##name(arg0)
#define KORE_GPU_CALL2(name, arg0, arg1) kore_d3d11_##name(arg0, arg1)
#define KORE_GPU_CALL3(name, arg0, arg1, arg2) kore_d3d11_##name(arg0, arg1, arg2)
#define KORE_GPU_CALL4(name, arg0, arg1, arg2, arg3) kore_d3d11_##name(arg0, arg1, arg2, arg3)
#define KORE_GPU_CALL5(name, arg0, arg1, arg2, arg3, arg4) kore_d3d11_##name(arg0, arg1, arg2, arg3, arg4)
#define KORE_GPU_CALL6(name, arg0, arg1, arg2, arg3, arg4, arg5) kore_d3d11_##name(arg0, arg1, arg2, arg3, arg4, arg5)
#define KORE_GPU_CALL7(name, arg0, arg1, arg2, arg3, arg4, arg5, arg6) kore_d3d11_##name(arg0, arg1, arg2, arg3, arg4, arg5, arg6)

#elif defined(KORE_DIRECT3D12)

#if defined(KORE_VULKAN)

#define KORE_GPU_IMPL(name)                                                                                                                                    \
	union {                                                                                                                                                    \
		kore_d3d12_##name d3d12;                                                                                                                               \
		kore_vulkan_##name vulkan;                                                                                                                             \
	}
#define KORE_GPU_CALL(name)                                                                                                                                    \
	switch (selected_api) {                                                                                                                                    \
	case KORE_GPU_API_DIRECT3D12:                                                                                                                              \
		kore_d3d12_##name();                                                                                                                                   \
		break;                                                                                                                                                 \
	case KORE_GPU_API_VULKAN:                                                                                                                                  \
		kore_vulkan_##name();                                                                                                                                  \
		break;                                                                                                                                                 \
	}
#define KORE_GPU_CALL1(name, arg0)                                                                                                                             \
	switch (selected_api) {                                                                                                                                    \
	case KORE_GPU_API_DIRECT3D12:                                                                                                                              \
		kore_d3d12_##name(arg0);                                                                                                                               \
		break;                                                                                                                                                 \
	case KORE_GPU_API_VULKAN:                                                                                                                                  \
		kore_vulkan_##name(arg0);                                                                                                                              \
		break;                                                                                                                                                 \
	}
#define KORE_GPU_CALL2(name, arg0, arg1)                                                                                                                       \
	switch (selected_api) {                                                                                                                                    \
	case KORE_GPU_API_DIRECT3D12:                                                                                                                              \
		kore_d3d12_##name(arg0, arg1);                                                                                                                         \
		break;                                                                                                                                                 \
	case KORE_GPU_API_VULKAN:                                                                                                                                  \
		kore_vulkan_##name(arg0, arg1);                                                                                                                        \
		break;                                                                                                                                                 \
	}
#define KORE_GPU_CALL3(name, arg0, arg1, arg2)                                                                                                                 \
	switch (selected_api) {                                                                                                                                    \
	case KORE_GPU_API_DIRECT3D12:                                                                                                                              \
		kore_d3d12_##name(arg0, arg1, arg2);                                                                                                                   \
		break;                                                                                                                                                 \
	case KORE_GPU_API_VULKAN:                                                                                                                                  \
		kore_vulkan_##name(arg0, arg1, arg2);                                                                                                                  \
		break;                                                                                                                                                 \
	}                                                                                                                                                          \
	#define KORE_G5_CALL4(name, arg0, arg1, arg2, arg3) switch (selected_api) {                                                                                \
	case KORE_GPU_API_DIRECT3D12:                                                                                                                              \
		kore_d3d12_##name(arg0, arg1, arg2, arg3);                                                                                                             \
		break;                                                                                                                                                 \
	case KORE_GPU_API_VULKAN:                                                                                                                                  \
		kore_vulkan_##name(arg0, arg1, arg2, arg3);                                                                                                            \
		break;                                                                                                                                                 \
	}

#else

#define KORE_GPU_IMPL(name) kore_d3d12_##name d3d12
#define KORE_GPU_CALL(name) kore_d3d12_##name()
#define KORE_GPU_CALL1(name, arg0) kore_d3d12_##name(arg0)
#define KORE_GPU_CALL2(name, arg0, arg1) kore_d3d12_##name(arg0, arg1)
#define KORE_GPU_CALL3(name, arg0, arg1, arg2) kore_d3d12_##name(arg0, arg1, arg2)
#define KORE_GPU_CALL4(name, arg0, arg1, arg2, arg3) kore_d3d12_##name(arg0, arg1, arg2, arg3)
#define KORE_GPU_CALL5(name, arg0, arg1, arg2, arg3, arg4) kore_d3d12_##name(arg0, arg1, arg2, arg3, arg4)
#define KORE_GPU_CALL6(name, arg0, arg1, arg2, arg3, arg4, arg5) kore_d3d12_##name(arg0, arg1, arg2, arg3, arg4, arg5)
#define KORE_GPU_CALL7(name, arg0, arg1, arg2, arg3, arg4, arg5, arg6) kore_d3d12_##name(arg0, arg1, arg2, arg3, arg4, arg5, arg6)

#endif

#elif defined(KORE_METAL)

#define KORE_GPU_IMPL(name) kore_metal_##name metal
#define KORE_GPU_CALL(name) kore_metal_##name()
#define KORE_GPU_CALL1(name, arg0) kore_metal_##name(arg0)
#define KORE_GPU_CALL2(name, arg0, arg1) kore_metal_##name(arg0, arg1)
#define KORE_GPU_CALL3(name, arg0, arg1, arg2) kore_metal_##name(arg0, arg1, arg2)
#define KORE_GPU_CALL4(name, arg0, arg1, arg2, arg3) kore_metal_##name(arg0, arg1, arg2, arg3)
#define KORE_GPU_CALL5(name, arg0, arg1, arg2, arg3, arg4) kore_metal_##name(arg0, arg1, arg2, arg3, arg4)
#define KORE_GPU_CALL6(name, arg0, arg1, arg2, arg3, arg4, arg5) kore_metal_##name(arg0, arg1, arg2, arg3, arg4, arg5)
#define KORE_GPU_CALL7(name, arg0, arg1, arg2, arg3, arg4, arg5, arg6) kore_metal_##name(arg0, arg1, arg2, arg3, arg4, arg5, arg6)

#elif defined(KORE_OPENGL)

#define KORE_GPU_IMPL(name) kore_opengl_##name opengl
#define KORE_GPU_CALL(name) kore_opengl_##name()
#define KORE_GPU_CALL1(name, arg0) kore_opengl_##name(arg0)
#define KORE_GPU_CALL2(name, arg0, arg1) kore_opengl_##name(arg0, arg1)
#define KORE_GPU_CALL3(name, arg0, arg1, arg2) kore_opengl_##name(arg0, arg1, arg2)
#define KORE_GPU_CALL4(name, arg0, arg1, arg2, arg3) kore_opengl_##name(arg0, arg1, arg2, arg3)
#define KORE_GPU_CALL5(name, arg0, arg1, arg2, arg3, arg4) kore_opengl_##name(arg0, arg1, arg2, arg3, arg4)
#define KORE_GPU_CALL6(name, arg0, arg1, arg2, arg3, arg4, arg5) kore_opengl_##name(arg0, arg1, arg2, arg3, arg4, arg5)
#define KORE_GPU_CALL7(name, arg0, arg1, arg2, arg3, arg4, arg5, arg6) kore_opengl_##name(arg0, arg1, arg2, arg3, arg4, arg5, arg6)

#elif defined(KORE_VULKAN)

#define KORE_GPU_IMPL(name) kore_vulkan_##name vulkan
#define KORE_GPU_CALL(name) kore_vulkan_##name()
#define KORE_GPU_CALL1(name, arg0) kore_vulkan_##name(arg0)
#define KORE_GPU_CALL2(name, arg0, arg1) kore_vulkan_##name(arg0, arg1)
#define KORE_GPU_CALL3(name, arg0, arg1, arg2) kore_vulkan_##name(arg0, arg1, arg2)
#define KORE_GPU_CALL4(name, arg0, arg1, arg2, arg3) kore_vulkan_##name(arg0, arg1, arg2, arg3)
#define KORE_GPU_CALL5(name, arg0, arg1, arg2, arg3, arg4) kore_vulkan_##name(arg0, arg1, arg2, arg3, arg4)
#define KORE_GPU_CALL6(name, arg0, arg1, arg2, arg3, arg4, arg5) kore_vulkan_##name(arg0, arg1, arg2, arg3, arg4, arg5)
#define KORE_GPU_CALL7(name, arg0, arg1, arg2, arg3, arg4, arg5, arg6) kore_vulkan_##name(arg0, arg1, arg2, arg3, arg4, arg5, arg6)

#elif defined(KORE_WEBGPU)

#define KORE_GPU_IMPL(name) kore_webgpu_##name webgpu
#define KORE_GPU_CALL(name) kore_webgpu_##name()
#define KORE_GPU_CALL1(name, arg0) kore_webgpu_##name(arg0)
#define KORE_GPU_CALL2(name, arg0, arg1) kore_webgpu_##name(arg0, arg1)
#define KORE_GPU_CALL3(name, arg0, arg1, arg2) kore_webgpu_##name(arg0, arg1, arg2)
#define KORE_GPU_CALL4(name, arg0, arg1, arg2, arg3) kore_webgpu_##name(arg0, arg1, arg2, arg3)
#define KORE_GPU_CALL5(name, arg0, arg1, arg2, arg3, arg4) kore_webgpu_##name(arg0, arg1, arg2, arg3, arg4)
#define KORE_GPU_CALL6(name, arg0, arg1, arg2, arg3, arg4, arg5) kore_webgpu_##name(arg0, arg1, arg2, arg3, arg4, arg5)
#define KORE_GPU_CALL7(name, arg0, arg1, arg2, arg3, arg4, arg5, arg6) kore_webgpu_##name(arg0, arg1, arg2, arg3, arg4, arg5, arg6)

#endif

#ifdef __cplusplus
}
#endif

#endif
