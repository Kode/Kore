#include <kore3/opengl/device_functions.h>

#include "openglunit.h"

#include <kore3/gpu/device.h>
#include <kore3/log.h>
#include <kore3/system.h>
#include <kore3/util/align.h>
#include <kore3/window.h>

#ifdef KORE_WINDOWS
#include <kore3/backend/windows.h>

#include <Windows.h>

#include <GL/wglew.h>
#endif

#ifdef KORE_LINUX
#define EGL_NO_PLATFORM_SPECIFIC_TYPES
#include <EGL/egl.h>
#endif

#include <assert.h>
#include <stdlib.h>

#ifdef KORE_WINDOWS
static HGLRC context;
static HDC   device_context;
#endif

#ifdef KORE_LINUX
EGLDisplay          kore_egl_get_display();
EGLNativeWindowType kore_egl_get_native_window(EGLDisplay, EGLConfig, int);

static EGLint     egl_major      = 0;
static EGLint     egl_minor      = 0;
static int        egl_depth_size = 0;
static EGLDisplay egl_display    = EGL_NO_DISPLAY;
static EGLContext egl_context    = EGL_NO_CONTEXT;
static EGLConfig  egl_config     = NULL;
static EGLSurface egl_surface;

#define kore_egl_check_errors()                                                          \
	{                                                                                    \
		EGLint error = eglGetError();                                                    \
		if (error != EGL_SUCCESS) {                                                      \
			kore_log(KORE_LOG_LEVEL_ERROR, "EGL Error at line %i: %i", __LINE__, error); \
			kore_debug_break();                                                          \
			exit(1);                                                                     \
		}                                                                                \
	}

static void kore_egl_init_window(int window) {
	egl_surface = eglCreateWindowSurface(egl_display, egl_config, kore_egl_get_native_window(egl_display, egl_config, window), NULL);
	kore_egl_check_errors();
	eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
	kore_egl_check_errors();
}

static void kore_egl_init(void) {
#if !defined(KORE_OPENGL_ES)
	eglBindAPI(EGL_OPENGL_API);
#else
	eglBindAPI(EGL_OPENGL_ES_API);
#endif

	egl_display = kore_egl_get_display();
	eglInitialize(egl_display, &egl_major, &egl_minor);
	kore_egl_check_errors();

	// clang-format off
	const EGLint attribs[] = {
		#if !defined(KORE_OPENGL_ES)
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		#else
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		#endif
		EGL_SURFACE_TYPE, 	 EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 		 8,
		EGL_GREEN_SIZE, 	 8,
		EGL_RED_SIZE, 		 8,
		EGL_DEPTH_SIZE, 	 24,
		EGL_STENCIL_SIZE, 	 8,
		EGL_NONE,
	};
	// clang-format on
	egl_depth_size = 24;

	EGLint num_configs = 0;
	eglChooseConfig(egl_display, attribs, &egl_config, 1, &num_configs);
	kore_egl_check_errors();

	if (!num_configs) {
		// clang-format off
		const EGLint attribs[] = {
			#if !defined(KORE_OPENGL_ES)
			EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
			#else
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			#endif
			EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
			EGL_BLUE_SIZE, 		 8,
			EGL_GREEN_SIZE, 	 8,
			EGL_RED_SIZE, 		 8,
			EGL_DEPTH_SIZE, 	 16,
			EGL_STENCIL_SIZE, 	 8,
			EGL_NONE,
		};
		// clang-format on
		eglChooseConfig(egl_display, attribs, &egl_config, 1, &num_configs);
		kore_egl_check_errors();
		egl_depth_size = 16;
	}

	if (!num_configs) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Unable to choose EGL config");
	}

#if !defined(KORE_OPENGL_ES)
	EGLint gl_versions[][2] = {{4, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1}, {4, 0}, {3, 3}, {3, 2}, {3, 1}, {3, 0}, {2, 1}, {2, 0}};
	bool   gl_initialized   = false;
	for (int i = 0; i < sizeof(gl_versions) / sizeof(EGLint) / 2; ++i) {
		{
			EGLint context_attribs[] = {EGL_CONTEXT_MAJOR_VERSION,
			                            gl_versions[i][0],
			                            EGL_CONTEXT_MINOR_VERSION,
			                            gl_versions[i][1],
			                            EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE,
			                            EGL_TRUE,
			                            EGL_NONE};
			egl_context              = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, context_attribs);
			EGLint error             = eglGetError();
			if (error == EGL_SUCCESS) {
				gl_initialized = true;
				kore_log(KORE_LOG_LEVEL_INFO, "Using OpenGL version %i.%i (forward-compatible).", gl_versions[i][0], gl_versions[i][1]);
				break;
			}
		}

		{
			EGLint context_attribs[] = {EGL_CONTEXT_MAJOR_VERSION, gl_versions[i][0], EGL_CONTEXT_MINOR_VERSION, gl_versions[i][1], EGL_NONE};
			egl_context              = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, context_attribs);
			EGLint error             = eglGetError();
			if (error == EGL_SUCCESS) {
				gl_initialized = true;
				kore_log(KORE_LOG_LEVEL_INFO, "Using OpenGL version %i.%i.", gl_versions[i][0], gl_versions[i][1]);
				break;
			}
		}
	}

	if (!gl_initialized) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Could not create OpenGL-context.");
		exit(1);
	}
#else
	EGLint context_attribs[] = {EGL_CONTEXT_MAJOR_VERSION, 2, EGL_NONE};
	egl_context              = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, context_attribs);
	kore_egl_check_errors();
#endif
}
#endif

static kore_gpu_texture framebuffer;
static GLuint           vertex_array;

#if defined(KORE_WINDOWS) && !defined(NDEBUG)
static void __stdcall debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
	kore_log(KORE_LOG_LEVEL_INFO, "OpenGL: %s", message);
}
#endif

void kore_opengl_device_create(kore_gpu_device *device, const kore_gpu_device_wishlist *wishlist) {
#ifdef KORE_WINDOWS
	HWND window_handle = kore_windows_window_handle(0);

	const uint32_t depth_buffer_bits   = 16;
	const uint32_t stencil_buffer_bits = 8;

	// clang-format off
	PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, (BYTE)depth_buffer_bits, (BYTE)stencil_buffer_bits, 0,
		PFD_MAIN_PLANE, 0, 0, 0, 0 };
	// clang-format on

	device_context      = GetDC(window_handle);
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

#if !defined(NDEBUG)
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(debug_callback, NULL);
#endif

#endif

#ifdef KORE_LINUX
	kore_egl_init();
	kore_egl_init_window(0);
#endif

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer.opengl.framebuffer);
	framebuffer.opengl.texture                = 0;
	framebuffer.opengl.is_primary_framebuffer = true;

	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	init_flip();
}

void kore_opengl_device_destroy(kore_gpu_device *device) {}

void kore_opengl_device_set_name(kore_gpu_device *device, const char *name) {}

void kore_opengl_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer) {
	glGenBuffers(1, &buffer->opengl.buffer);

	if ((parameters->usage_flags & KORE_OPENGL_BUFFER_USAGE_VERTEX) != 0) {
		buffer->opengl.buffer_type = GL_ARRAY_BUFFER;
	}
	else if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_INDEX) != 0) {
		buffer->opengl.buffer_type = GL_ELEMENT_ARRAY_BUFFER;
	}
	else if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_READ) != 0) {
		buffer->opengl.buffer_type = GL_PIXEL_PACK_BUFFER;
	}
	else {
		assert(false);
	}

	buffer->opengl.size = parameters->size;

	glBindBuffer(buffer->opengl.buffer_type, buffer->opengl.buffer);
	kore_opengl_check_errors();
	glBufferData(buffer->opengl.buffer_type, parameters->size, NULL, GL_STATIC_DRAW);
	glBindBuffer(buffer->opengl.buffer_type, 0);

#ifdef KORE_WEBGL
	buffer->opengl.data = malloc(parameters->size);
#endif
}

void kore_opengl_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {
	list->opengl.commands        = malloc(1024 * 1024);
	list->opengl.commands_offset = 0;
}

void kore_opengl_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {}

kore_gpu_texture *kore_opengl_device_get_framebuffer(kore_gpu_device *device) {
	return &framebuffer;
}

kore_gpu_texture_format kore_opengl_device_framebuffer_format(kore_gpu_device *device) {
	return KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM;
}

void kore_opengl_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {
	kore_gpu_index_format index_format     = KORE_GPU_INDEX_FORMAT_UINT16;
	kore_gpu_texture_view framebuffer_view = {0};

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

			// TODO
			glEnableVertexAttribArray(0);
			kore_opengl_check_errors();

			glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void *)(int64_t)0);
			kore_opengl_check_errors();

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

			if (framebuffer_view.texture->opengl.is_primary_framebuffer) {
				glUseProgram(data->pipeline->program);
			}
			else {
				glUseProgram(data->pipeline->flip_program);
			}
			kore_opengl_check_errors();

			break;
		}
		case COMMAND_COPY_TEXTURE_TO_BUFFER: {
			copy_texture_to_buffer *data = (copy_texture_to_buffer *)&c->data;

			if (data->source->texture->opengl.is_primary_framebuffer) {
				// read framebuffer into the buffer
				glBindBuffer(data->destination->buffer->opengl.buffer_type, data->destination->buffer->opengl.buffer);
				glReadPixels(0, 0, data->width, data->height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glBindBuffer(data->destination->buffer->opengl.buffer_type, 0);

				kore_opengl_check_errors();

				// create texture backed by the buffer
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, data->destination->buffer->opengl.buffer);
				uint32_t texture;
				glGenTextures(1, &texture);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data->width, data->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glBindTexture(GL_TEXTURE_2D, 0);

				kore_opengl_check_errors();

				uint32_t flipped_framebuffer = flip(data->width, data->height, texture);

				// read the flipped framebuffer into the buffer
				glBindFramebuffer(GL_FRAMEBUFFER, flipped_framebuffer);
				glBindBuffer(data->destination->buffer->opengl.buffer_type, data->destination->buffer->opengl.buffer);
				glReadPixels(0, 0, data->width, data->height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glBindBuffer(data->destination->buffer->opengl.buffer_type, 0);

				kore_opengl_check_errors();
			}
			else {
				assert(false);
			}

			break;
		}
		case COMMAND_BEGIN_RENDER_PASS: {
			begin_render_pass *data = (begin_render_pass *)&c->data;
			framebuffer_view        = data->parameters.color_attachments[0].texture;
			break;
		}
		case COMMAND_END_RENDER_PASS: {
			break;
		}
		case COMMAND_PRESENT: {
#if defined(KORE_WINDOWS)
			SwapBuffers(device_context);
#elif defined(KORE_LINUX)
			if (!eglSwapBuffers(egl_display, egl_surface)) {
				EGLint error = eglGetError();
				if (error == EGL_BAD_SURFACE) {
					kore_log(KORE_LOG_LEVEL_WARNING, "Recreating surface.");
					kore_egl_init_window(0);
				}
				else if (error == EGL_CONTEXT_LOST || error == EGL_BAD_CONTEXT) {
					kore_log(KORE_LOG_LEVEL_ERROR, "Context lost.");
				}
			}
#endif
		}
		}

		kore_opengl_check_errors();

		offset += c->size;
	}

	list->opengl.commands_offset = 0;
}

void kore_opengl_device_wait_until_idle(kore_gpu_device *device) {
	glFlush();
}

void kore_opengl_device_create_descriptor_set(kore_gpu_device *device, uint32_t descriptor_count, uint32_t dynamic_descriptor_count,
                                              uint32_t bindless_descriptor_count, uint32_t sampler_count, kore_opengl_descriptor_set *set) {}

void kore_opengl_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler) {}

void kore_opengl_device_create_raytracing_volume(kore_gpu_device *device, kore_gpu_buffer *vertex_buffer, uint64_t vertex_count, kore_gpu_buffer *index_buffer,
                                                 uint32_t index_count, kore_gpu_raytracing_volume *volume) {}

void kore_opengl_device_create_raytracing_hierarchy(kore_gpu_device *device, kore_gpu_raytracing_volume **volumes, kore_matrix4x4 *volume_transforms,
                                                    uint32_t volumes_count, kore_gpu_raytracing_hierarchy *hierarchy) {}

void kore_opengl_device_create_query_set(kore_gpu_device *device, const kore_gpu_query_set_parameters *parameters, kore_gpu_query_set *query_set) {}

uint32_t kore_opengl_device_align_texture_row_bytes(kore_gpu_device *device, uint32_t row_bytes) {
	return row_bytes;
}

void kore_opengl_device_create_fence(kore_gpu_device *device, kore_gpu_fence *fence) {}

void kore_opengl_device_signal(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}

void kore_opengl_device_wait(kore_gpu_device *device, kore_gpu_command_list_type list_type, kore_gpu_fence *fence, uint64_t value) {}
