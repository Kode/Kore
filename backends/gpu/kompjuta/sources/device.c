#include <kore3/kompjuta/device_functions.h>

#include <kore3/gpu/commandlist.h>
#include <kore3/gpu/device.h>
#include <kore3/util/align.h>

#include <kore3/log.h>
#include <kore3/window.h>

#include <stdlib.h>

#include <kore3/backend/mmio.h>

void kore_kompjuta_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist) {}

void kore_kompjuta_device_destroy(kore_gpu_device *device) {}

void kore_kompjuta_device_set_name(kore_gpu_device *device, const char *name) {}

void kore_kompjuta_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer) {
	buffer->kompjuta.data = malloc(parameters->size);
}

void kore_kompjuta_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {
	list->kompjuta.commands_count  = 1024;
	list->kompjuta.commands        = malloc(sizeof(kompjuta_gpu_command) * list->kompjuta.commands_count);
	list->kompjuta.current_command = 0;
}

void kore_kompjuta_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {}

kore_gpu_texture *kore_kompjuta_device_get_framebuffer(kore_gpu_device *device) {
	return NULL;
}

kore_gpu_texture_format kore_kompjuta_device_framebuffer_format(kore_gpu_device *device) {
	return KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM;
}

void kore_kompjuta_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {
	uint8_t *mmio = (uint8_t *)MMIO_BASE;

	uint64_t *list_address = (uint64_t *)&mmio[COMMAND_LIST_ADDR];
	*list_address          = (uint64_t)list->kompjuta.commands;

	uint32_t *list_size = (uint32_t *)&mmio[COMMAND_LIST_SIZE];
	*list_size          = list->kompjuta.current_command;

	uint8_t *execute = &mmio[EXECUTE_COMMAND_LIST];
	*execute         = 1;

	list->kompjuta.current_command = 0;
}

void kore_kompjuta_device_wait_until_idle(kore_gpu_device *device) {}

void kore_kompjuta_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler) {}

void kore_kompjuta_device_create_raytracing_volume(kore_gpu_device *device, kore_gpu_buffer *vertex_buffer, uint64_t vertex_count,
                                                   kore_gpu_buffer *index_buffer, uint32_t index_count, kore_gpu_raytracing_volume *volume) {}

void kore_kompjuta_device_create_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_volume **volumes, kore_matrix4x4 *volume_transforms,
                                                      uint32_t volumes_count, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_kompjuta_device_create_query_set(kore_gpu_device *device, const kore_gpu_query_set_parameters *parameters, kore_gpu_query_set *query_set) {}

uint32_t kore_kompjuta_device_align_texture_row_bytes(kore_gpu_device *device, uint32_t row_bytes) {
	return (uint32_t)align_pow2((int)row_bytes, 256);
}

void kore_kompjuta_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence) {}

void kore_kompjuta_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}

void kore_kompjuta_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}
