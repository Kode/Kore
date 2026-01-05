#include <kore3/framebuffer/framebuffer.h>

#include <kore3/gpu/buffer.h>
#include <kore3/gpu/device.h>

#include <kong.h>

static kore_gpu_device       framebuffer_device;
static kore_gpu_command_list framebuffer_list;
static kore_gpu_buffer       framebuffer_buffer;

uint32_t kore_framebuffer_width;
uint32_t kore_framebuffer_height;
uint32_t kore_framebuffer_stride;

uint8_t *kore_framebuffer_pixels;

void kore_fb_init(uint32_t width, uint32_t height) {
	kore_gpu_device_wishlist wishlist = {0};
	kore_gpu_device_create(&framebuffer_device, &wishlist);

	kore_framebuffer_width  = width;
	kore_framebuffer_height = height;
	kore_framebuffer_stride = kore_gpu_device_align_texture_row_bytes(&framebuffer_device, width * 4);

	kong_init(&framebuffer_device);

	kore_gpu_device_create_command_list(&framebuffer_device, KORE_GPU_COMMAND_LIST_TYPE_GRAPHICS, &framebuffer_list);

	kore_gpu_buffer_parameters parameters = {
	    .size        = kore_framebuffer_height * kore_framebuffer_stride,
	    .usage_flags = KORE_GPU_BUFFER_USAGE_CPU_WRITE | KORE_GPU_BUFFER_USAGE_COPY_SRC,
	};
	kore_gpu_device_create_buffer(&framebuffer_device, &parameters, &framebuffer_buffer);
}

void kore_fb_begin(void) {
	kore_framebuffer_pixels = (uint8_t *)kore_gpu_buffer_lock_all(&framebuffer_buffer);
}

void kore_fb_end(void) {
	kore_gpu_buffer_unlock(&framebuffer_buffer);

	kore_gpu_texture *gpu_framebuffer = kore_gpu_device_get_framebuffer(&framebuffer_device);

	kore_gpu_color clear_color = {
	    .r = 0.0f,
	    .g = 0.0f,
	    .b = 0.0f,
	    .a = 1.0f,
	};

	kore_gpu_render_pass_parameters parameters = {
	    .color_attachments_count = 1,
	    .color_attachments =
	        {
	            {
	                .load_op     = KORE_GPU_LOAD_OP_CLEAR,
	                .clear_value = clear_color,
	                .texture =
	                    {
	                        .texture           = gpu_framebuffer,
	                        .array_layer_count = 1,
	                        .mip_level_count   = 1,
	                        .format            = kore_gpu_device_framebuffer_format(&framebuffer_device),
	                        .dimension         = KORE_GPU_TEXTURE_VIEW_DIMENSION_2D,
	                    },
	            },
	        },
	};
	kore_gpu_command_list_begin_render_pass(&framebuffer_list, &parameters);

	kore_gpu_command_list_end_render_pass(&framebuffer_list);

	kore_gpu_image_copy_buffer copy_buffer = {
	    .buffer         = &framebuffer_buffer,
	    .bytes_per_row  = kore_framebuffer_stride,
	    .offset         = 0,
	    .rows_per_image = kore_framebuffer_height,
	};

	kore_gpu_image_copy_texture copy_texture = {
	    .texture   = gpu_framebuffer,
	    .origin_x  = 0,
	    .origin_y  = 0,
	    .origin_z  = 0,
	    .mip_level = 0,
	    .aspect    = KORE_GPU_IMAGE_COPY_ASPECT_ALL,
	};

	kore_gpu_command_list_copy_buffer_to_texture(&framebuffer_list, &copy_buffer, &copy_texture, kore_framebuffer_width, kore_framebuffer_height, 1);

	kore_gpu_command_list_present(&framebuffer_list);

	kore_gpu_device_execute_command_list(&framebuffer_device, &framebuffer_list);
}
