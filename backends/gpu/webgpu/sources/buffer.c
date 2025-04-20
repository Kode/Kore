#include <kore3/webgpu/buffer_functions.h>

#include <kore3/gpu/buffer.h>

#include <emscripten.h>

static bool mapped = false;

static void map_async_callback(WGPUBufferMapAsyncStatus status, void *userdata) {
	mapped = true;
}

static void map_async_no_callback(WGPUBufferMapAsyncStatus status, void *userdata) {
	
}

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
	return buffer->webgpu.locked_data;
}

void *kore_webgpu_buffer_try_to_lock(kore_gpu_buffer *buffer, uint64_t offset, uint64_t size) {
	if (buffer->webgpu.has_copy_buffer) {
		if (wgpuBufferGetMapState(buffer->webgpu.copy_buffer) == WGPUBufferMapState_Mapped) {
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
	return buffer->webgpu.locked_data;
}

void kore_webgpu_buffer_unlock(kore_gpu_buffer *buffer) {
	if (buffer->webgpu.has_copy_buffer) {
		wgpuBufferUnmap(buffer->webgpu.copy_buffer);
		buffer->webgpu.copy_scheduled = true;
	}
	else {
		wgpuBufferUnmap(buffer->webgpu.buffer);
	}
}
