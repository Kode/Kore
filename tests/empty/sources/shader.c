#include <kore3/gpu/device.h>
#include <kore3/io/filereader.h>
#include <kore3/system.h>

#include <kong.h>

#include <assert.h>
#include <stdlib.h>

static kore_gpu_device       device;
static kore_gpu_command_list list;

static const int width  = 800;
static const int height = 600;

static void update(void *data) {
	kore_gpu_texture *framebuffer = kore_gpu_device_get_framebuffer(&device);

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
	                        .texture           = framebuffer,
	                        .array_layer_count = 1,
	                        .mip_level_count   = 1,
	                        .format            = KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM,
	                        .dimension         = KORE_GPU_TEXTURE_VIEW_DIMENSION_2D,
	                    },
	            },
	        },
	};
	kore_gpu_command_list_begin_render_pass(&list, &parameters);

	kore_gpu_command_list_end_render_pass(&list);

	kore_gpu_command_list_present(&list);

	kore_gpu_device_execute_command_list(&device, &list);
}

int kickstart(int argc, char **argv) {
	kore_init("Example", width, height, NULL, NULL);
	kore_set_update_callback(update, NULL);

	kore_gpu_device_wishlist wishlist = {0};
	kore_gpu_device_create(&device, &wishlist);

	kong_init(&device);

	kore_gpu_device_create_command_list(&device, KORE_GPU_COMMAND_LIST_TYPE_GRAPHICS, &list);

	kore_start();

	return 0;
}
