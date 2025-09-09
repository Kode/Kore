#include <kore3/direct3d12/texture_functions.h>

#include "d3d12unit.h"

#include <kore3/gpu/texture.h>

uint32_t kore_d3d12_texture_resource_state_index(kore_gpu_texture *texture, uint32_t mip_level, uint32_t array_layer) {
	return mip_level + (array_layer * texture->d3d12.mip_level_count);
}

void kore_d3d12_texture_set_name(kore_gpu_texture *texture, const char *name) {
	wchar_t wstr[1024];
	kore_microsoft_convert_string(wstr, name, 1024);
	COM_CALL1(texture->d3d12.resource, SetName, wstr);
}

void kore_d3d12_texture_destroy(kore_gpu_texture *texture) {
	kore_d3d12_device_destroy_texture(texture->d3d12.device, texture);
}

bool kore_d3d12_texture_in_use(kore_gpu_texture *texture) {
	return !check_for_fence(texture->d3d12.device->d3d12.execution_fence, texture->d3d12.execution_index);
}

void kore_d3d12_texture_wait_until_not_in_use(kore_gpu_texture *texture) {
	wait_for_fence(texture->d3d12.device, texture->d3d12.device->d3d12.execution_fence, texture->d3d12.device->d3d12.execution_event,
	               texture->d3d12.execution_index);
}
