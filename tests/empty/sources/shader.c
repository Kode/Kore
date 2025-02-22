#include <kinc/io/filereader.h>
#include <kinc/system.h>
#include <kore3/gpu/device.h>

#include <kong.h>

#include <assert.h>
#include <stdlib.h>

static kore_gpu_device device;
static kore_gpu_command_list list;

static const int width = 800;
static const int height = 600;

static void update(void *data) {
	kore_gpu_texture *framebuffer = kore_gpu_device_get_framebuffer(&device);

	kore_gpu_render_pass_parameters parameters = {0};
	parameters.color_attachments_count = 1;
	parameters.color_attachments[0].load_op = KORE_GPU_LOAD_OP_CLEAR;
	kore_gpu_color clear_color;
	clear_color.r = 0.0f;
	clear_color.g = 0.0f;
	clear_color.b = 0.0f;
	clear_color.a = 1.0f;
	parameters.color_attachments[0].clear_value = clear_color;
	parameters.color_attachments[0].texture.texture = framebuffer;
	parameters.color_attachments[0].texture.array_layer_count = 1;
	parameters.color_attachments[0].texture.mip_level_count = 1;
	parameters.color_attachments[0].texture.format = KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM;
	parameters.color_attachments[0].texture.dimension = KORE_GPU_TEXTURE_VIEW_DIMENSION_2D;
	kore_gpu_command_list_begin_render_pass(&list, &parameters);

	kore_gpu_command_list_end_render_pass(&list);

	kore_gpu_command_list_present(&list);

	kore_gpu_device_execute_command_list(&device, &list);
}

int kickstart(int argc, char **argv) {
	kinc_init("Example", width, height, NULL, NULL);
	kinc_set_update_callback(update, NULL);

	kore_gpu_device_wishlist wishlist = {0};
	kore_gpu_device_create(&device, &wishlist);

	kong_init(&device);

	kore_gpu_device_create_command_list(&device, KORE_GPU_COMMAND_LIST_TYPE_GRAPHICS, &list);

	kinc_start();

	return 0;
}
