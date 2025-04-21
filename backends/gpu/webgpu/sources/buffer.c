#include <kore3/webgpu/buffer_functions.h>

#include <kore3/gpu/buffer.h>

#include <emscripten.h>

#include <string.h>

static void kore_webgpu_buffer_schedule_uploads(kore_webgpu_buffer *buffer) {
	for (uint8_t region_index = 0; region_index < buffer->outdated_regions_count; ++region_index) {
		assert(scheduled_buffer_uploads_count < 256);

		buffer_upload *upload = &scheduled_buffer_uploads[scheduled_buffer_uploads_count];

		upload->buffer = buffer;
		upload->offset = buffer->outdated_regions[region_index].offset;
		upload->size   = buffer->outdated_regions[region_index].size;

		scheduled_buffer_uploads_count += 1;
	}

	buffer->outdated_regions_count = 0;
}

static void kore_webgpu_outdate_region(kore_webgpu_buffer *buffer, uint64_t offset, uint64_t size) {
	for (uint8_t region_index = 0; region_index < buffer->outdated_regions_count; ++region_index) {
		kore_webgpu_buffer_region *region = &buffer->outdated_regions[region_index];
		if (offset >= region->offset && offset + size <= region->offset + region->size) {
			return;
		}
	}

	kore_webgpu_buffer_region new_regions[KORE_WEBGPU_MAX_OUTDATED_REGIONS];
	new_regions[0].offset = offset;
	new_regions[0].size   = size;
	uint8_t regions_count = 1;

	for (uint8_t region_index = 0; region_index < buffer->outdated_regions_count; ++region_index) {
		kore_webgpu_buffer_region *region = &buffer->outdated_regions[region_index];

		if (region->offset >= offset && region->offset + region->size <= offset + size) {
			continue;
		}

		assert(regions_count < KORE_WEBGPU_MAX_OUTDATED_REGIONS);

		new_regions[regions_count] = *region;
		regions_count += 1;
	}

	memcpy(&buffer->outdated_regions, new_regions, sizeof(new_regions));
	buffer->outdated_regions_count = regions_count;
}

static bool mapped = false;

static void map_async_callback(WGPUBufferMapAsyncStatus status, void *userdata) {
	mapped = true;
}

static void map_async_no_callback(WGPUBufferMapAsyncStatus status, void *userdata) {}

static void map_async(WGPUBuffer buffer, WGPUMapMode map_mode, uint64_t offset, uint64_t size) {
	if (wgpuBufferGetMapState(buffer) == WGPUBufferMapState_Mapped) {
		return;
	}

	mapped = false;
	wgpuBufferMapAsync(buffer, map_mode, offset, size, map_async_callback, NULL);
	while (!mapped) {
		emscripten_sleep(0);
	}
}

void kore_webgpu_buffer_set_name(kore_gpu_buffer *buffer, const char *name) {}

void kore_webgpu_buffer_destroy(kore_gpu_buffer *buffer) {}

void *kore_webgpu_buffer_try_to_lock_all(kore_gpu_buffer *buffer) {
	if (buffer->webgpu.has_copy_buffer) {
		if (wgpuBufferGetMapState(buffer->webgpu.copy_buffer) == WGPUBufferMapState_Mapped) {
			buffer->webgpu.last_lock_offset = 0;
			buffer->webgpu.last_lock_size   = buffer->webgpu.size;

			buffer->webgpu.locked_data = wgpuBufferGetMappedRange(buffer->webgpu.copy_buffer, 0, buffer->webgpu.size);
		}
		else {
			if (wgpuBufferGetMapState(buffer->webgpu.copy_buffer) == WGPUBufferMapState_Unmapped) {
				wgpuBufferMapAsync(buffer->webgpu.copy_buffer, WGPUMapMode_Write, 0, buffer->webgpu.size, map_async_no_callback, NULL);
			}
			buffer->webgpu.locked_data = NULL;
		}
	}
	else {
		if (buffer->webgpu.write) {
			if (wgpuBufferGetMapState(buffer->webgpu.buffer) == WGPUBufferMapState_Mapped) {
				buffer->webgpu.last_lock_offset = 0;
				buffer->webgpu.last_lock_size   = buffer->webgpu.size;

				buffer->webgpu.locked_data = wgpuBufferGetMappedRange(buffer->webgpu.buffer, 0, buffer->webgpu.size);
			}
			else {
				if (wgpuBufferGetMapState(buffer->webgpu.buffer) == WGPUBufferMapState_Unmapped) {
					wgpuBufferMapAsync(buffer->webgpu.buffer, WGPUMapMode_Write, 0, buffer->webgpu.size, map_async_no_callback, NULL);
				}
				buffer->webgpu.locked_data = NULL;
			}
		}
		else {
			if (wgpuBufferGetMapState(buffer->webgpu.buffer) == WGPUBufferMapState_Mapped) {
				buffer->webgpu.last_lock_offset = 0;
				buffer->webgpu.last_lock_size   = buffer->webgpu.size;

				buffer->webgpu.locked_data = (void *)wgpuBufferGetConstMappedRange(buffer->webgpu.buffer, 0, buffer->webgpu.size);
			}
			else {
				if (wgpuBufferGetMapState(buffer->webgpu.buffer) == WGPUBufferMapState_Unmapped) {
					wgpuBufferMapAsync(buffer->webgpu.buffer, WGPUMapMode_Read, 0, buffer->webgpu.size, map_async_no_callback, NULL);
				}
				buffer->webgpu.locked_data = NULL;
			}
		}
	}
	return buffer->webgpu.locked_data;
}

void *kore_webgpu_buffer_lock_all(kore_gpu_buffer *buffer) {
	if (buffer->webgpu.has_copy_buffer) {
		map_async(buffer->webgpu.copy_buffer, WGPUMapMode_Write, 0, buffer->webgpu.size);
		buffer->webgpu.locked_data = wgpuBufferGetMappedRange(buffer->webgpu.copy_buffer, 0, buffer->webgpu.size);
	}
	else {
		if (buffer->webgpu.write) {
			map_async(buffer->webgpu.buffer, WGPUMapMode_Write, 0, buffer->webgpu.size);
			buffer->webgpu.locked_data = wgpuBufferGetMappedRange(buffer->webgpu.buffer, 0, buffer->webgpu.size);
		}
		else {
			map_async(buffer->webgpu.buffer, WGPUMapMode_Read, 0, buffer->webgpu.size);
			buffer->webgpu.locked_data = (void *)wgpuBufferGetConstMappedRange(buffer->webgpu.buffer, 0, buffer->webgpu.size);
		}
	}

	buffer->webgpu.last_lock_offset = 0;
	buffer->webgpu.last_lock_size   = buffer->webgpu.size;

	return buffer->webgpu.locked_data;
}

void *kore_webgpu_buffer_try_to_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	if (buffer->webgpu.has_copy_buffer) {
		if (wgpuBufferGetMapState(buffer->webgpu.copy_buffer) == WGPUBufferMapState_Mapped) {
			buffer->webgpu.last_lock_offset = offset;
			buffer->webgpu.last_lock_size   = size;

			buffer->webgpu.locked_data = wgpuBufferGetMappedRange(buffer->webgpu.copy_buffer, offset, size);
		}
		else {
			if (wgpuBufferGetMapState(buffer->webgpu.copy_buffer) == WGPUBufferMapState_Unmapped) {
				wgpuBufferMapAsync(buffer->webgpu.copy_buffer, WGPUMapMode_Write, offset, size, map_async_no_callback, NULL);
			}
			buffer->webgpu.locked_data = NULL;
		}
	}
	else {
		if (buffer->webgpu.write) {
			if (wgpuBufferGetMapState(buffer->webgpu.buffer) == WGPUBufferMapState_Mapped) {
				buffer->webgpu.last_lock_offset = offset;
				buffer->webgpu.last_lock_size   = size;

				buffer->webgpu.locked_data = wgpuBufferGetMappedRange(buffer->webgpu.buffer, offset, size);
			}
			else {
				if (wgpuBufferGetMapState(buffer->webgpu.buffer) == WGPUBufferMapState_Unmapped) {
					wgpuBufferMapAsync(buffer->webgpu.buffer, WGPUMapMode_Write, offset, size, map_async_no_callback, NULL);
				}
				buffer->webgpu.locked_data = NULL;
			}
		}
		else {
			if (wgpuBufferGetMapState(buffer->webgpu.buffer) == WGPUBufferMapState_Mapped) {
				buffer->webgpu.last_lock_offset = offset;
				buffer->webgpu.last_lock_size   = size;

				buffer->webgpu.locked_data = (void *)wgpuBufferGetConstMappedRange(buffer->webgpu.buffer, offset, size);
			}
			else {
				if (wgpuBufferGetMapState(buffer->webgpu.buffer) == WGPUBufferMapState_Unmapped) {
					wgpuBufferMapAsync(buffer->webgpu.buffer, WGPUMapMode_Read, offset, size, map_async_no_callback, NULL);
				}
				buffer->webgpu.locked_data = NULL;
			}
		}
	}
	return buffer->webgpu.locked_data;
}

void *kore_webgpu_buffer_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	if (buffer->webgpu.has_copy_buffer) {
		map_async(buffer->webgpu.copy_buffer, WGPUMapMode_Write, offset, size);
		buffer->webgpu.locked_data = wgpuBufferGetMappedRange(buffer->webgpu.copy_buffer, offset, size);
	}
	else {
		if (buffer->webgpu.write) {
			map_async(buffer->webgpu.buffer, WGPUMapMode_Write, offset, size);
			buffer->webgpu.locked_data = wgpuBufferGetMappedRange(buffer->webgpu.buffer, offset, size);
		}
		else {
			map_async(buffer->webgpu.buffer, WGPUMapMode_Read, offset, size);
			buffer->webgpu.locked_data = (void *)wgpuBufferGetConstMappedRange(buffer->webgpu.buffer, offset, size);
		}
	}

	buffer->webgpu.last_lock_offset = offset;
	buffer->webgpu.last_lock_size   = size;

	return buffer->webgpu.locked_data;
}

void kore_webgpu_buffer_unlock(kore_gpu_buffer *buffer) {
	if (buffer->webgpu.has_copy_buffer) {
		wgpuBufferUnmap(buffer->webgpu.copy_buffer);
		kore_webgpu_outdate_region(&buffer->webgpu, buffer->webgpu.last_lock_offset, buffer->webgpu.last_lock_size);
	}
	else {
		wgpuBufferUnmap(buffer->webgpu.buffer);
	}
}
