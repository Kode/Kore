#include "webgpuunit.h"

#include <kore3/gpu/device.h>

#include <assert.h>

#include "buffer.c"
#include "commandlist.c"
#include "descriptorset.c"
#include "device.c"
#include "pipeline.c"
#include "sampler.c"
#include "raytracing.c"
#include "texture.c"

static WGPUCompareFunction convert_compare(kore_gpu_compare_function func) {
	switch (func) {
	case KORE_GPU_COMPARE_FUNCTION_UNDEFINED:
		return WGPUCompareFunction_Undefined;
	case KORE_GPU_COMPARE_FUNCTION_NEVER:
		return WGPUCompareFunction_Never;
	case KORE_GPU_COMPARE_FUNCTION_LESS:
		return WGPUCompareFunction_Less;
	case KORE_GPU_COMPARE_FUNCTION_EQUAL:
		return WGPUCompareFunction_Equal;
	case KORE_GPU_COMPARE_FUNCTION_LESS_EQUAL:
		return WGPUCompareFunction_LessEqual;
	case KORE_GPU_COMPARE_FUNCTION_GREATER:
		return WGPUCompareFunction_Greater;
	case KORE_GPU_COMPARE_FUNCTION_NOT_EQUAL:
		return WGPUCompareFunction_NotEqual;
	case KORE_GPU_COMPARE_FUNCTION_GREATER_EQUAL:
		return WGPUCompareFunction_GreaterEqual;
	case KORE_GPU_COMPARE_FUNCTION_ALWAYS:
		return WGPUCompareFunction_Always;
	}
}
