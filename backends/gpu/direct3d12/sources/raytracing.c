#include <kore3/direct3d12/raytracing_functions.h>

#include "d3d12unit.h"

#include <kore3/gpu/device.h>

void kore_d3d12_raytracing_volume_destroy(kore_gpu_raytracing_volume *volume) {
	kore_d3d12_device_destroy_raytracing_volume(volume->d3d12.device, volume);
}

void kore_d3d12_raytracing_hierarchy_destroy(kore_gpu_raytracing_hierarchy *hierarchy) {
	kore_d3d12_device_destroy_raytracing_hierarchy(hierarchy->d3d12.device, hierarchy);
}

bool kore_d3d12_raytracing_volume_in_use(kore_gpu_raytracing_volume *volume) {
	return !check_for_fence(volume->d3d12.device->d3d12.execution_fence, volume->d3d12.execution_index);
}

void kore_d3d12_raytracing_volume_wait_until_not_in_use(kore_gpu_raytracing_volume *volume) {
	wait_for_fence(volume->d3d12.device, volume->d3d12.device->d3d12.execution_fence, volume->d3d12.device->d3d12.execution_event,
	               volume->d3d12.execution_index);
}

bool kore_d3d12_raytracing_hierarchy_in_use(kore_gpu_raytracing_hierarchy *hierarchy) {
	return !check_for_fence(hierarchy->d3d12.device->d3d12.execution_fence, hierarchy->d3d12.execution_index);
}

void kore_d3d12_raytracing_hierarchy_wait_until_not_in_use(kore_gpu_raytracing_hierarchy *hierarchy) {
	wait_for_fence(hierarchy->d3d12.device, hierarchy->d3d12.device->d3d12.execution_fence, hierarchy->d3d12.device->d3d12.execution_event,
	               hierarchy->d3d12.execution_index);
}
