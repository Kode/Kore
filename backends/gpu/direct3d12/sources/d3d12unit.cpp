#include "d3d12unit.h"

#include <kore3/gpu/device.h>

#include <assert.h>

static D3D12_COMPARISON_FUNC convert_compare_function(kore_gpu_compare_function fun) {
	switch (fun) {
	case KORE_GPU_COMPARE_FUNCTION_NEVER:
		return D3D12_COMPARISON_FUNC_NEVER;
	case KORE_GPU_COMPARE_FUNCTION_LESS:
		return D3D12_COMPARISON_FUNC_LESS;
	case KORE_GPU_COMPARE_FUNCTION_EQUAL:
		return D3D12_COMPARISON_FUNC_EQUAL;
	case KORE_GPU_COMPARE_FUNCTION_LESS_EQUAL:
		return D3D12_COMPARISON_FUNC_LESS_EQUAL;
	case KORE_GPU_COMPARE_FUNCTION_GREATER:
		return D3D12_COMPARISON_FUNC_GREATER;
	case KORE_GPU_COMPARE_FUNCTION_NOT_EQUAL:
		return D3D12_COMPARISON_FUNC_NOT_EQUAL;
	case KORE_GPU_COMPARE_FUNCTION_GREATER_EQUAL:
		return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
	case KORE_GPU_COMPARE_FUNCTION_ALWAYS:
		return D3D12_COMPARISON_FUNC_ALWAYS;
	default:
		assert(false);
		return D3D12_COMPARISON_FUNC_ALWAYS;
	}
}

static D3D12_FILTER convert_filter(kore_gpu_filter_mode minification, kore_gpu_filter_mode magnification, kore_gpu_mipmap_filter_mode mipmap) {
	switch (minification) {
	case KORE_GPU_FILTER_MODE_NEAREST:
		switch (magnification) {
		case KORE_GPU_FILTER_MODE_NEAREST:
			switch (mipmap) {
			case KORE_GPU_MIPMAP_FILTER_MODE_NEAREST:
				return D3D12_FILTER_MIN_MAG_MIP_POINT;
			case KORE_GPU_MIPMAP_FILTER_MODE_LINEAR:
				return D3D12_FILTER_MIN_MAG_POINT_MIP_LINEAR;
			}
		case KORE_GPU_FILTER_MODE_LINEAR:
			switch (mipmap) {
			case KORE_GPU_MIPMAP_FILTER_MODE_NEAREST:
				return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			case KORE_GPU_MIPMAP_FILTER_MODE_LINEAR:
				return D3D12_FILTER_MIN_POINT_MAG_MIP_LINEAR;
			}
		}
	case KORE_GPU_FILTER_MODE_LINEAR:
		switch (magnification) {
		case KORE_GPU_FILTER_MODE_NEAREST:
			switch (mipmap) {
			case KORE_GPU_MIPMAP_FILTER_MODE_NEAREST:
				return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			case KORE_GPU_MIPMAP_FILTER_MODE_LINEAR:
				return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			}
		case KORE_GPU_FILTER_MODE_LINEAR:
			switch (mipmap) {
			case KORE_GPU_MIPMAP_FILTER_MODE_NEAREST:
				return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			case KORE_GPU_MIPMAP_FILTER_MODE_LINEAR:
				return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			}
		}
	}

	assert(false);
	return D3D12_FILTER_MIN_MAG_MIP_POINT;
}

#include "buffer.cpp"
#include "commandlist.cpp"
#include "descriptorset.cpp"
#include "device.cpp"
#include "pipeline.cpp"
#include "sampler.cpp"
#include "texture.cpp"
