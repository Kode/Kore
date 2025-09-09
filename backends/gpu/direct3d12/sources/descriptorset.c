#include <kore3/direct3d12/descriptorset_functions.h>
#include <kore3/direct3d12/descriptorset_structs.h>

#include <kore3/direct3d12/texture_functions.h>

#include <kore3/error.h>
#include <kore3/util/align.h>

void kore_d3d12_descriptor_set_set_buffer_view_cbv(kore_gpu_device *device, kore_d3d12_descriptor_set *set, kore_gpu_buffer *buffer, uint32_t index) {
	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {0};
	desc.BufferLocation                  = COM_CALL0(buffer->d3d12.resource, GetGPUVirtualAddress);
	desc.SizeInBytes                     = align_pow2((int)buffer->d3d12.size, 256);

	D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle;
	COM_CALL0RET(device->d3d12.descriptor_heap, GetCPUDescriptorHandleForHeapStart, descriptor_handle);
	descriptor_handle.ptr += (set->allocations[set->current_allocation_index].descriptor_allocation.offset + index) * device->d3d12.cbv_srv_uav_increment;

	COM_CALL2(device->d3d12.device, CreateConstantBufferView, &desc, descriptor_handle);
}

void kore_d3d12_descriptor_set_set_buffer_view_srv(kore_gpu_device *device, kore_d3d12_descriptor_set *set, kore_gpu_buffer *buffer, uint32_t index) {
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {0};
	desc.ViewDimension                   = D3D12_SRV_DIMENSION_BUFFER;
	desc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Format                          = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.FirstElement             = 0;
	desc.Buffer.NumElements              = 1;
	desc.Buffer.StructureByteStride      = (UINT)buffer->d3d12.size;
	desc.Buffer.Flags                    = D3D12_BUFFER_SRV_FLAG_NONE;

	D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle;
	COM_CALL0RET(device->d3d12.descriptor_heap, GetCPUDescriptorHandleForHeapStart, descriptor_handle);
	descriptor_handle.ptr += (set->allocations[set->current_allocation_index].descriptor_allocation.offset + index) * device->d3d12.cbv_srv_uav_increment;

	COM_CALL3(device->d3d12.device, CreateShaderResourceView, buffer->d3d12.resource, &desc, descriptor_handle);
}

void kore_d3d12_descriptor_set_set_buffer_view_uav(kore_gpu_device *device, kore_d3d12_descriptor_set *set, kore_gpu_buffer *buffer, uint32_t index) {
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {0};
	desc.ViewDimension                    = D3D12_UAV_DIMENSION_BUFFER;
	desc.Format                           = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.FirstElement              = 0;
	desc.Buffer.NumElements               = 1;
	desc.Buffer.StructureByteStride       = (UINT)buffer->d3d12.size;
	desc.Buffer.CounterOffsetInBytes      = 0;
	desc.Buffer.Flags                     = D3D12_BUFFER_UAV_FLAG_NONE;

	D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle;
	COM_CALL0RET(device->d3d12.descriptor_heap, GetCPUDescriptorHandleForHeapStart, descriptor_handle);
	descriptor_handle.ptr += (set->allocations[set->current_allocation_index].descriptor_allocation.offset + index) * device->d3d12.cbv_srv_uav_increment;

	COM_CALL4(device->d3d12.device, CreateUnorderedAccessView, buffer->d3d12.resource, NULL, &desc, descriptor_handle);
}

void kore_d3d12_descriptor_set_set_bvh_view_srv(kore_gpu_device *device, kore_d3d12_descriptor_set *set, kore_gpu_raytracing_hierarchy *bvh, uint32_t index) {
	D3D12_SHADER_RESOURCE_VIEW_DESC desc          = {0};
	desc.ViewDimension                            = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
	desc.Shader4ComponentMapping                  = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.Format                                   = DXGI_FORMAT_UNKNOWN;
	desc.RaytracingAccelerationStructure.Location = COM_CALL0(bvh->d3d12.acceleration_structure.d3d12.resource, GetGPUVirtualAddress);

	D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle;
	COM_CALL0RET(device->d3d12.descriptor_heap, GetCPUDescriptorHandleForHeapStart, descriptor_handle);
	descriptor_handle.ptr += (set->allocations[set->current_allocation_index].descriptor_allocation.offset + index) * device->d3d12.cbv_srv_uav_increment;

	COM_CALL3(device->d3d12.device, CreateShaderResourceView, NULL, &desc, descriptor_handle);
}

void kore_d3d12_descriptor_set_set_texture_view_srv(kore_gpu_device *device, uint32_t offset, const kore_gpu_texture_view *texture_view) {
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {0};
	desc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	DXGI_FORMAT format = (DXGI_FORMAT)texture_view->texture->d3d12.format;
	switch (format) {
	case DXGI_FORMAT_D16_UNORM:
		desc.Format = DXGI_FORMAT_R16_UNORM;
		break;
	case DXGI_FORMAT_D32_FLOAT:
		desc.Format = DXGI_FORMAT_R32_FLOAT;
		break;
	default:
		desc.Format = format;
		break;
	}

	desc.Texture2D.MipLevels           = texture_view->mip_level_count;
	desc.Texture2D.MostDetailedMip     = texture_view->base_mip_level;
	desc.Texture2D.ResourceMinLODClamp = 0.0f;

	D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle;
	COM_CALL0RET(device->d3d12.descriptor_heap, GetCPUDescriptorHandleForHeapStart, descriptor_handle);
	descriptor_handle.ptr += offset * device->d3d12.cbv_srv_uav_increment;

	COM_CALL3(device->d3d12.device, CreateShaderResourceView, texture_view->texture->d3d12.resource, &desc, descriptor_handle);
}

void kore_d3d12_descriptor_set_set_texture_array_view_srv(kore_gpu_device *device, kore_d3d12_descriptor_set *set, const kore_gpu_texture_view *texture_view,
                                                          uint32_t index) {
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {0};
	desc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	desc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	DXGI_FORMAT format = (DXGI_FORMAT)texture_view->texture->d3d12.format;
	switch (format) {
	case DXGI_FORMAT_D16_UNORM:
		desc.Format = DXGI_FORMAT_R16_UNORM;
		break;
	default:
		desc.Format = format;
		break;
	}

	desc.Texture2DArray.MipLevels           = texture_view->mip_level_count;
	desc.Texture2DArray.MostDetailedMip     = texture_view->base_mip_level;
	desc.Texture2DArray.ResourceMinLODClamp = 0.0f;
	desc.Texture2DArray.FirstArraySlice     = texture_view->base_array_layer;
	desc.Texture2DArray.ArraySize           = texture_view->array_layer_count;

	D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle;
	COM_CALL0RET(device->d3d12.descriptor_heap, GetCPUDescriptorHandleForHeapStart, descriptor_handle);
	descriptor_handle.ptr += (set->allocations[set->current_allocation_index].descriptor_allocation.offset + index) * device->d3d12.cbv_srv_uav_increment;

	COM_CALL3(device->d3d12.device, CreateShaderResourceView, texture_view->texture->d3d12.resource, &desc, descriptor_handle);
}

void kore_d3d12_descriptor_set_set_texture_cube_view_srv(kore_gpu_device *device, kore_d3d12_descriptor_set *set, const kore_gpu_texture_view *texture_view,
                                                         uint32_t index) {
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {0};
	desc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURECUBE;
	desc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	DXGI_FORMAT format = (DXGI_FORMAT)texture_view->texture->d3d12.format;
	switch (format) {
	case DXGI_FORMAT_D16_UNORM:
		desc.Format = DXGI_FORMAT_R16_UNORM;
		break;
	default:
		desc.Format = format;
		break;
	}

	desc.TextureCube.MipLevels           = texture_view->mip_level_count;
	desc.TextureCube.MostDetailedMip     = texture_view->base_mip_level;
	desc.TextureCube.ResourceMinLODClamp = 0.0f;

	D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle;
	COM_CALL0RET(device->d3d12.descriptor_heap, GetCPUDescriptorHandleForHeapStart, descriptor_handle);
	descriptor_handle.ptr += (set->allocations[set->current_allocation_index].descriptor_allocation.offset + index) * device->d3d12.cbv_srv_uav_increment;

	COM_CALL3(device->d3d12.device, CreateShaderResourceView, texture_view->texture->d3d12.resource, &desc, descriptor_handle);
}

void kore_d3d12_descriptor_set_set_texture_view_uav(kore_gpu_device *device, kore_d3d12_descriptor_set *set, const kore_gpu_texture_view *texture_view,
                                                    uint32_t index) {
	D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {0};
	desc.ViewDimension                    = D3D12_UAV_DIMENSION_TEXTURE2D;
	desc.Format                           = (DXGI_FORMAT)texture_view->texture->d3d12.format;
	desc.Texture2D.MipSlice               = texture_view->base_mip_level;
	desc.Texture2D.PlaneSlice             = 0;

	D3D12_CPU_DESCRIPTOR_HANDLE descriptor_handle;
	COM_CALL0RET(device->d3d12.descriptor_heap, GetCPUDescriptorHandleForHeapStart, descriptor_handle);
	descriptor_handle.ptr += (set->allocations[set->current_allocation_index].descriptor_allocation.offset + index) * device->d3d12.cbv_srv_uav_increment;

	COM_CALL4(device->d3d12.device, CreateUnorderedAccessView, texture_view->texture->d3d12.resource, NULL, &desc, descriptor_handle);
}

void kore_d3d12_descriptor_set_set_sampler(kore_gpu_device *device, kore_d3d12_descriptor_set *set, kore_gpu_sampler *sampler, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE src_handle;
	COM_CALL0RET(device->d3d12.all_samplers, GetCPUDescriptorHandleForHeapStart, src_handle);
	src_handle.ptr += sampler->d3d12.sampler_index * device->d3d12.sampler_increment;

	D3D12_CPU_DESCRIPTOR_HANDLE dst_handle;
	COM_CALL0RET(device->d3d12.sampler_heap, GetCPUDescriptorHandleForHeapStart, dst_handle);
	dst_handle.ptr += (set->allocations[set->current_allocation_index].sampler_allocation.offset + index) * device->d3d12.sampler_increment;

	COM_CALL4(device->d3d12.device, CopyDescriptorsSimple, 1, dst_handle, src_handle, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
}

void kore_d3d12_descriptor_set_prepare_cbv_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint32_t offset, uint32_t size) {
	if (buffer->d3d12.resource_state != D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER && buffer->d3d12.resource_state != D3D12_RESOURCE_STATE_GENERIC_READ) {
		D3D12_RESOURCE_BARRIER barrier;
		barrier.Transition.pResource   = buffer->d3d12.resource;
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.StateBefore = (D3D12_RESOURCE_STATES)buffer->d3d12.resource_state;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		COM_CALL2(list->d3d12.list, ResourceBarrier, 1, &barrier);

		buffer->d3d12.resource_state = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	}

	// if (buffer->d3d12.cpu_read || buffer->d3d12.cpu_write) {
	kore_d3d12_command_list_queue_buffer_access(list, &buffer->d3d12, offset, size);
	//}
}

void kore_d3d12_descriptor_set_prepare_uav_buffer(kore_gpu_command_list *list, kore_gpu_buffer *buffer, uint32_t offset, uint32_t size) {
	if (buffer->d3d12.resource_state != D3D12_RESOURCE_STATE_UNORDERED_ACCESS) {
		D3D12_RESOURCE_BARRIER barrier;
		barrier.Transition.pResource   = buffer->d3d12.resource;
		barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.StateBefore = (D3D12_RESOURCE_STATES)buffer->d3d12.resource_state;
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		COM_CALL2(list->d3d12.list, ResourceBarrier, 1, &barrier);

		buffer->d3d12.resource_state = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	}

	// if (buffer->d3d12.cpu_read || buffer->d3d12.cpu_write) {
	kore_d3d12_command_list_queue_buffer_access(list, &buffer->d3d12, offset, size);
	//}
}

void kore_d3d12_descriptor_set_prepare_srv_texture(kore_gpu_command_list *list, const kore_gpu_texture_view *texture_view) {
	for (uint32_t array_layer = texture_view->base_array_layer; array_layer < texture_view->base_array_layer + texture_view->array_layer_count; ++array_layer) {
		for (uint32_t mip_level = texture_view->base_mip_level; mip_level < texture_view->base_mip_level + texture_view->mip_level_count; ++mip_level) {
			if (texture_view->texture->d3d12.resource_states[kore_d3d12_texture_resource_state_index(texture_view->texture, mip_level, array_layer)] !=
			    (D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)) {
				D3D12_RESOURCE_BARRIER barrier;
				barrier.Transition.pResource = texture_view->texture->d3d12.resource;
				barrier.Type                 = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				barrier.Flags                = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				barrier.Transition.StateBefore =
				    (D3D12_RESOURCE_STATES)
				        texture_view->texture->d3d12.resource_states[kore_d3d12_texture_resource_state_index(texture_view->texture, mip_level, array_layer)];
				barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
				barrier.Transition.Subresource = D3D12CalcSubresource(mip_level, array_layer, 0, texture_view->texture->d3d12.mip_level_count,
				                                                      texture_view->texture->d3d12.depth_or_array_layers);

				COM_CALL2(list->d3d12.list, ResourceBarrier, 1, &barrier);

				texture_view->texture->d3d12.resource_states[kore_d3d12_texture_resource_state_index(texture_view->texture, mip_level, array_layer)] =
				    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
			}
		}
	}

	kore_d3d12_command_list_queue_texture_access(list, &texture_view->texture->d3d12);
}

void kore_d3d12_descriptor_set_prepare_uav_texture(kore_gpu_command_list *list, const kore_gpu_texture_view *texture_view) {
	if (texture_view->texture->d3d12.in_flight_frame_index > 0) {
		list->d3d12.blocking_frame_index = texture_view->texture->d3d12.in_flight_frame_index;
	}

	if (texture_view->texture->d3d12.resource_states[kore_d3d12_texture_resource_state_index(texture_view->texture, texture_view->base_mip_level, 0)] !=
	    D3D12_RESOURCE_STATE_UNORDERED_ACCESS) {
		D3D12_RESOURCE_BARRIER barrier;
		barrier.Transition.pResource = texture_view->texture->d3d12.resource;
		barrier.Type                 = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags                = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.StateBefore =
		    (D3D12_RESOURCE_STATES)
		        texture_view->texture->d3d12.resource_states[kore_d3d12_texture_resource_state_index(texture_view->texture, texture_view->base_mip_level, 0)];
		if (list->d3d12.list_type == D3D12_COMMAND_LIST_TYPE_COMPUTE && barrier.Transition.StateBefore == D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) {
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		}
		barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		barrier.Transition.Subresource = D3D12CalcSubresource(texture_view->base_mip_level, 0, 0, texture_view->texture->d3d12.mip_level_count,
		                                                      texture_view->texture->d3d12.depth_or_array_layers);

		COM_CALL2(list->d3d12.list, ResourceBarrier, 1, &barrier);

		texture_view->texture->d3d12.resource_states[kore_d3d12_texture_resource_state_index(texture_view->texture, texture_view->base_mip_level, 0)] =
		    D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	}

	kore_d3d12_command_list_queue_texture_access(list, &texture_view->texture->d3d12);
}

void kore_d3d312_desciptor_set_use_free_allocation(kore_d3d12_descriptor_set *set) {
	for (uint32_t allocation_index = 0; allocation_index < KORE_D3D12_ALLOCATIONS_PER_DESCRIPTORSET; ++allocation_index) {
		kore_d3d12_descriptor_set_allocation *allocation = &set->allocations[allocation_index];

		if (allocation->command_list_reference_count == 0 && allocation->execution_index <= COM_CALL0(set->device->d3d12.execution_fence, GetCompletedValue)) {
			set->current_allocation_index = allocation_index;
			return;
		}
	}

	uint32_t oldest_allocation_index           = UINT32_MAX;
	uint64_t oldest_allocation_execution_index = UINT64_MAX;

	for (uint32_t allocation_index = 0; allocation_index < KORE_D3D12_ALLOCATIONS_PER_DESCRIPTORSET; ++allocation_index) {
		kore_d3d12_descriptor_set_allocation *allocation = &set->allocations[allocation_index];

		if (allocation->command_list_reference_count == 0) {
			if (allocation->execution_index < oldest_allocation_execution_index) {
				oldest_allocation_index           = allocation_index;
				oldest_allocation_execution_index = allocation->execution_index;
			}
		}
	}

	if (oldest_allocation_index == UINT32_MAX) {
		kore_error_message("Descriptor set stall");
	}

	{
		kore_d3d12_descriptor_set_allocation *allocation = &set->allocations[oldest_allocation_index];
		wait_for_fence(set->device, set->device->d3d12.execution_fence, set->device->d3d12.execution_event, allocation->execution_index);
		set->current_allocation_index = oldest_allocation_index;
	}
}

void kore_d3d12_descriptor_set_destroy(kore_d3d12_descriptor_set *set) {
	kore_d3d12_device_destroy_descriptor_set(set->device, set);
}
