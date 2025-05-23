#include <kore3/graphics4/graphics.h>
#include <kore3/graphics4/indexbuffer.h>
#include <kore3/graphics4/pipeline.h>
#include <kore3/graphics4/shader.h>
#include <kore3/graphics4/vertexbuffer.h>
#include <kore3/input/mouse.h>
#include <kore3/io/filereader.h>
#include <kore3/log.h>
#include <kore3/system.h>
#include <kore3/window.h>

#include <assert.h>
#include <stdlib.h>

static kore_g4_shader_t        vertex_shader;
static kore_g4_shader_t        fragment_shader;
static kore_g4_pipeline_t      pipeline;
static kore_g4_vertex_buffer_t vertices;
static kore_g4_index_buffer_t  indices;

#define WINDOW_COUNT 2

static struct window {
	int  index;
	bool open;
	bool mouse_down;
} windows[WINDOW_COUNT] = {0};

#define HEAP_SIZE 1024 * 1024
static uint8_t *heap     = NULL;
static size_t   heap_top = 0;

static void *allocate(size_t size) {
	size_t old_top = heap_top;
	heap_top += size;
	assert(heap_top <= HEAP_SIZE);
	return &heap[old_top];
}

static void update(void *data) {
	for (int window = 0; window < WINDOW_COUNT; window++) {
		if (windows[window].open) {
			kore_g4_begin(window);
			kore_g4_clear(KORE_G4_CLEAR_COLOR, windows[window].mouse_down ? 0xff0000ff : 0, 0.0f, 0);

			kore_g4_set_pipeline(&pipeline);
			kore_g4_set_vertex_buffer(&vertices);
			kore_g4_set_index_buffer(&indices);
			kore_g4_draw_indexed_vertices();

			kore_g4_end(window);
		}
	}
	kore_g4_swap_buffers();
}

static void load_shader(const char *filename, kore_g4_shader_t *shader, kore_g4_shader_type_t shader_type) {
	kore_file_reader_t file;
	kore_file_reader_open(&file, filename, KORE_FILE_TYPE_ASSET);
	size_t   data_size = kore_file_reader_size(&file);
	uint8_t *data      = allocate(data_size);
	kore_file_reader_read(&file, data, data_size);
	kore_file_reader_close(&file);
	kore_g4_shader_init(shader, data, data_size, shader_type);
}

static void mouse_down(int window, int button, int x, int y) {
	windows[window].mouse_down = true;
}

static void mouse_up(int window, int button, int x, int y) {
	windows[window].mouse_down = false;
}

static bool window_close(void *data) {
	struct window *window = data;
	window->open          = false;
	return true;
}

static char *copy_callback(void *data) {
	return "Hello World!";
}

static void paste_callback(char *text, void *data) {
	kore_log(KORE_LOG_LEVEL_INFO, "Pasted: %s", text);
}

static void drop_files_callback(wchar_t *text, void *data) {
	char dest[1024];
	wcstombs(dest, text, sizeof(dest));
	kore_log(KORE_LOG_LEVEL_INFO, "Dropped: %s", dest);
}

int kickstart(int argc, char **argv) {
	kore_window_options_t first_window_options;
	kore_window_options_set_defaults(&first_window_options);
	// first_window_options.mode = KORE_WINDOW_MODE_FULLSCREEN;
	int first_window = kore_init("MultiWindow", 1024, 768, &first_window_options, NULL);
	// return 0;
	kore_set_update_callback(update, NULL);

	heap = (uint8_t *)malloc(HEAP_SIZE);
	assert(heap != NULL);

	load_shader("shader.vert", &vertex_shader, KORE_G4_SHADER_TYPE_VERTEX);
	load_shader("shader.frag", &fragment_shader, KORE_G4_SHADER_TYPE_FRAGMENT);

	kore_g4_vertex_structure_t structure;
	kore_g4_vertex_structure_init(&structure);
	kore_g4_vertex_structure_add(&structure, "pos", KORE_G4_VERTEX_DATA_FLOAT3);
	kore_g4_pipeline_init(&pipeline);
	pipeline.vertex_shader   = &vertex_shader;
	pipeline.fragment_shader = &fragment_shader;
	pipeline.input_layout[0] = &structure;
	pipeline.input_layout[1] = NULL;
	kore_g4_pipeline_compile(&pipeline);

	kore_g4_vertex_buffer_init(&vertices, 3, &structure, KORE_G4_USAGE_STATIC, 0);
	{
		float *v = kore_g4_vertex_buffer_lock_all(&vertices);
		int    i = 0;

		v[i++] = -1;
		v[i++] = -1;
		v[i++] = 0.5;

		v[i++] = 1;
		v[i++] = -1;
		v[i++] = 0.5;

		v[i++] = -1;
		v[i++] = 1;
		v[i++] = 0.5;

		kore_g4_vertex_buffer_unlock_all(&vertices);
	}

	kore_g4_index_buffer_init(&indices, 3, KORE_G4_INDEX_BUFFER_FORMAT_32BIT, KORE_G4_USAGE_STATIC);
	{
		uint32_t *i = (uint32_t *)kore_g4_index_buffer_lock_all(&indices);
		i[0]        = 0;
		i[1]        = 1;
		i[2]        = 2;
		kore_g4_index_buffer_unlock_all(&indices);
	}
	kore_window_options_t      options;
	kore_framebuffer_options_t frame_options;
	kore_window_options_set_defaults(&options);
	kore_framebuffer_options_set_defaults(&frame_options);
	int window_two   = kore_window_create(&options, &frame_options);
	windows[0].index = 0;
	windows[0].open  = true;
	windows[1].index = 1;
	windows[1].open  = true;
	kore_window_set_close_callback(first_window, window_close, &windows[0]);
	kore_window_set_close_callback(window_two, window_close, &windows[1]);

	kore_mouse_set_press_callback(mouse_down);
	kore_mouse_set_release_callback(mouse_up);
	kore_set_copy_callback(copy_callback, NULL);
	kore_set_paste_callback(paste_callback, NULL);
	kore_set_drop_files_callback(drop_files_callback, NULL);
	kore_start();

	return 0;
}