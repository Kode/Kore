#include <kore3/opengl/device_functions.h>

#include "openglunit.h"

#include <kore3/gpu/device.h>
#include <kore3/util/align.h>

#include <kore3/log.h>
#include <kore3/window.h>

#ifdef KORE_WINDOWS
#include <kore3/backend/windows.h>

#include <Windows.h>

#include <GL/wglew.h>
#endif

#include <assert.h>

#ifdef KORE_WINDOWS
static HGLRC context;
static HDC device_context;
#endif

static GLint framebuffer;
static GLuint vertex_array;

#if defined(KORE_WINDOWS) && !defined(NDEBUG)
static void __stdcall debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
	kore_log(KORE_LOG_LEVEL_INFO, "OpenGL: %s", message);
}
#endif

void kore_opengl_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist) {
#ifdef KORE_WINDOWS
	HWND window_handle = kore_windows_window_handle(0);

	const uint32_t depth_buffer_bits = 16;
	const uint32_t stencil_buffer_bits = 8;

	// clang-format off
	PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (BYTE)depth_buffer_bits, (BYTE)stencil_buffer_bits, 0,
		PFD_MAIN_PLANE, 0, 0, 0, 0 };
	// clang-format on

	device_context = GetDC(window_handle);
	GLuint pixel_format = ChoosePixelFormat(device_context, &pfd);
	SetPixelFormat(device_context, pixel_format, &pfd);
	HGLRC temp_context = wglCreateContext(device_context);
	wglMakeCurrent(device_context, temp_context);

	glewInit();

	if (wglewIsSupported("WGL_ARB_create_context") == 1) {
		// clang-format off
		int attributes[] = { WGL_CONTEXT_MAJOR_VERSION_ARB, 4, WGL_CONTEXT_MINOR_VERSION_ARB, 2, WGL_CONTEXT_FLAGS_ARB,
			WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB, WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, 0 };
		// clang-format on

		context = wglCreateContextAttribsARB(device_context, 0, attributes);
		kore_opengl_check_errors();

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(temp_context);
	}
	else {
		context = temp_context;
	}

	wglMakeCurrent(device_context, context);
	kore_opengl_check_errors();

	if (wglSwapIntervalEXT != NULL) {
		wglSwapIntervalEXT(true);
	}

#if defined(KORE_WINDOWS) && !defined(NDEBUG)
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(debug_callback, NULL);
#endif

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer);
#endif

	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);
}

void kore_opengl_device_destroy(kore_gpu_device *device) {}

void kore_opengl_device_set_name(kore_gpu_device *device, const char *name) {}

void kore_opengl_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer) {
	glGenBuffers(1, &buffer->opengl.buffer);

	if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_VERTEX) != 0) {
		buffer->opengl.buffer_type = GL_ARRAY_BUFFER;
	}
	else if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_INDEX) != 0) {
		buffer->opengl.buffer_type = GL_ELEMENT_ARRAY_BUFFER;
	}
	else {
		assert(false);
	}

	buffer->opengl.size = parameters->size;

	glBindBuffer(buffer->opengl.buffer_type, buffer->opengl.buffer);
	kore_opengl_check_errors();
	glBufferData(buffer->opengl.buffer_type, parameters->size, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(buffer->opengl.buffer_type, 0);
}

void kore_opengl_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {
	list->opengl.commands = malloc(1024 * 1024);
	list->opengl.commands_offset = 0;
	list->opengl.presents = false;
}

void kore_opengl_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {}

kore_gpu_texture *kore_opengl_device_get_framebuffer(kore_gpu_device *device) {
	return NULL;
}

kore_gpu_texture_format kore_opengl_device_framebuffer_format(kore_gpu_device *device) {
	return KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM;
}

void kore_opengl_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {
	kore_gpu_index_format index_format = KORE_GPU_INDEX_FORMAT_UINT16;

	uint64_t offset = 0;

	while (offset < list->opengl.commands_offset) {
		command *c = (command *)&list->opengl.commands[offset];

		switch (c->type) {
		case COMMAND_SET_INDEX_BUFFER: {
			set_index_buffer_data *data = (set_index_buffer_data *)&c->data;

			index_format = data->index_format;
			glBindBuffer(data->buffer->opengl.buffer_type, data->buffer->opengl.buffer);

			break;
		}
		case COMMAND_SET_VERTEX_BUFFER: {
			set_vertex_buffer_data *data = (set_vertex_buffer_data *)&c->data;

			glBindBuffer(data->buffer->buffer_type, data->buffer->buffer);

			break;
		}
		case COMMAND_DRAW_INDEXED: {
			draw_indexed_data *data = (draw_indexed_data *)&c->data;

			void *start =
			    index_format == KORE_GPU_INDEX_FORMAT_UINT16 ? (void *)(data->first_index * sizeof(uint16_t)) : (void *)(data->first_index * sizeof(uint32_t));
			glDrawElements(GL_TRIANGLES, data->index_count, index_format == KORE_GPU_INDEX_FORMAT_UINT16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, start);

			break;
		}
		case COMMAND_SET_RENDER_PIPELINE: {
			set_render_pipeline *data = (set_render_pipeline *)&c->data;

			glUseProgram(data->pipeline->program);
			kore_opengl_check_errors();

			break;
		}
		}

		kore_opengl_check_errors();

		offset += c->size;
	}

	list->opengl.commands_offset = 0;

	if (list->opengl.presents) {
#ifdef KORE_WINDOWS
		SwapBuffers(device_context);
#endif
	}
}

void kore_opengl_device_wait_until_idle(kore_gpu_device *device) {}

void kore_opengl_device_create_descriptor_set(kore_gpu_device *device, uint32_t descriptor_count, uint32_t dynamic_descriptor_count,
                                              uint32_t bindless_descriptor_count, uint32_t sampler_count, kore_opengl_descriptor_set *set) {}

void kore_opengl_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler) {}

void kore_opengl_device_create_raytracing_volume(kore_gpu_device *device, kore_gpu_buffer *vertex_buffer, uint64_t vertex_count, kore_gpu_buffer *index_buffer,
                                                 uint32_t index_count, kore_gpu_raytracing_volume *volume) {}

void kore_opengl_device_create_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_volume **volumes, kore_matrix4x4 *volume_transforms,
                                                    uint32_t volumes_count, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_opengl_device_create_query_set(kore_gpu_device *device, const kore_gpu_query_set_parameters *parameters, kore_gpu_query_set *query_set) {}

uint32_t kore_opengl_device_align_texture_row_bytes(kore_gpu_device *device, uint32_t row_bytes) {
	return 0;
}

void kore_opengl_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence) {}

void kore_opengl_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}

void kore_opengl_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}
