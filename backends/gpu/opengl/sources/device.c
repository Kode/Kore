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
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(debug_callback, NULL);
#endif

#endif

#ifdef KORE_LINUX
	kore_egl_init();
	kore_egl_init_window(0);
#endif

	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &device->opengl.default_framebuffer);

	const kore_gpu_texture_parameters texture_parameters = {
	    .width                 = kore_window_width(0),
	    .height                = kore_window_height(0),
	    .format                = KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM,
	    .dimension             = KORE_GPU_TEXTURE_DIMENSION_2D,
	    .depth_or_array_layers = 1,
	    .mip_level_count       = 1,
	    .sample_count          = 1,
	    .usage                 = KORE_GPU_TEXTURE_USAGE_RENDER_ATTACHMENT | KORE_GPU_TEXTURE_USAGE_COPY_DST,
	};
	kore_gpu_device_create_texture(device, &texture_parameters, &framebuffer);
	framebuffer.opengl.is_primary_framebuffer = true;

	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);
	kore_opengl_check_errors();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	kore_opengl_check_errors();

	glGenFramebuffers(1, &device->opengl.custom_framebuffer);

	init_copy();
	init_flip();
}

void kore_opengl_device_destroy(kore_gpu_device *device) {}

void kore_opengl_device_set_name(kore_gpu_device *device, const char *name) {}

static uint32_t next_uniform_buffer_index = 0;

void kore_opengl_device_create_buffer(kore_gpu_device *device, const kore_gpu_buffer_parameters *parameters, kore_gpu_buffer *buffer) {
	glGenBuffers(1, &buffer->opengl.buffer);

	bool dynamic = false;

	if ((parameters->usage_flags & KORE_OPENGL_BUFFER_USAGE_VERTEX) != 0) {
		buffer->opengl.buffer_type = GL_ARRAY_BUFFER;
	}
	else if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_INDEX) != 0) {
		buffer->opengl.buffer_type = GL_ELEMENT_ARRAY_BUFFER;
	}
	else if ((parameters->usage_flags & KORE_OPENGL_BUFFER_USAGE_UNIFORM) != 0) {
		buffer->opengl.uniform_buffer = next_uniform_buffer_index;
		glBindBufferBase(GL_UNIFORM_BUFFER, buffer->opengl.uniform_buffer, buffer->opengl.buffer);
		next_uniform_buffer_index += 1;
		buffer->opengl.buffer_type = GL_UNIFORM_BUFFER;

		dynamic = true;
	}
	else if ((parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_READ) != 0 || (parameters->usage_flags & KORE_GPU_BUFFER_USAGE_CPU_WRITE) != 0) {
		buffer->opengl.buffer_type = GL_PIXEL_PACK_BUFFER;
		dynamic                    = true;
	}
	else {
		assert(false);
	}

	buffer->opengl.size = parameters->size;

	glBindBuffer(buffer->opengl.buffer_type, buffer->opengl.buffer);
	kore_opengl_check_errors();
	glBufferData(buffer->opengl.buffer_type, parameters->size, NULL, dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	glBindBuffer(buffer->opengl.buffer_type, 0);

#ifdef KORE_WEBGL
	buffer->opengl.locked_data = malloc(parameters->size);
#endif
}

void kore_opengl_device_create_command_list(kore_gpu_device *device, kore_gpu_command_list_type type, kore_gpu_command_list *list) {
	list->opengl.commands        = malloc(1024 * 1024);
	list->opengl.commands_offset = 0;
}

static GLenum convert_internal_format(kore_gpu_texture_format format) {
	switch (format) {
	case KORE_GPU_TEXTURE_FORMAT_UNDEFINED:
		assert(false);
		return GL_R8_SNORM;
	case KORE_GPU_TEXTURE_FORMAT_R8_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_R8_SNORM:
		return GL_R8_SNORM;
	case KORE_GPU_TEXTURE_FORMAT_R8_UINT:
	case KORE_GPU_TEXTURE_FORMAT_R8_SINT:
		return GL_R8;
	case KORE_GPU_TEXTURE_FORMAT_R16_UINT:
	case KORE_GPU_TEXTURE_FORMAT_R16_SINT:
	case KORE_GPU_TEXTURE_FORMAT_R16_FLOAT:
#ifdef GL_R16
		return GL_R16;
#else
		return GL_RED;
#endif
	case KORE_GPU_TEXTURE_FORMAT_RG8_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_RG8_SNORM:
		return GL_RG8_SNORM;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RG8_SINT:
		return GL_RG8;
	case KORE_GPU_TEXTURE_FORMAT_R32_UINT:
		return GL_R32UI;
	case KORE_GPU_TEXTURE_FORMAT_R32_SINT:
		return GL_R32I;
	case KORE_GPU_TEXTURE_FORMAT_R32_FLOAT:
		return GL_R32F;
	case KORE_GPU_TEXTURE_FORMAT_RG16_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RG16_SINT:
	case KORE_GPU_TEXTURE_FORMAT_RG16_FLOAT:
#ifdef GL_RG16
		return GL_RG16;
#else
		return GL_RED;
#endif
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM_SRGB:
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SNORM:
		return GL_RGBA8_SNORM;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SINT:
		return GL_RGBA8;
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM_SRGB:
		return GL_RGBA8_SNORM;
	case KORE_GPU_TEXTURE_FORMAT_RGB9E5U_FLOAT:
		return GL_RGB9_E5;
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_RG11B10U_FLOAT:
		assert(false);
		return GL_RGBA8;
	case KORE_GPU_TEXTURE_FORMAT_RG32_UINT:
		return GL_RG32UI;
	case KORE_GPU_TEXTURE_FORMAT_RG32_SINT:
		return GL_RG32I;
	case KORE_GPU_TEXTURE_FORMAT_RG32_FLOAT:
		return GL_RG32F;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_UINT:
		return GL_RGBA16UI;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_SINT:
		return GL_RGBA16I;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_FLOAT:
		return GL_RGBA16F;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_UINT:
		return GL_RGBA32UI;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_SINT:
		return GL_RGBA32I;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_FLOAT:
		return GL_RGBA32F;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH16_UNORM:
#ifdef GL_DEPTH_COMPONENT24
		return GL_DEPTH_COMPONENT16;
#else
		return GL_RED;
#endif
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24_NOTHING8:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24_STENCIL8:
#ifdef GL_DEPTH_COMPONENT24
		return GL_DEPTH_COMPONENT24;
#else
		return GL_RED;
#endif
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32_FLOAT:
#ifdef GL_DEPTH_COMPONENT32
		return GL_DEPTH_COMPONENT32;
#else
		return GL_RED;
#endif
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32_FLOAT_STENCIL8_NOTHING24:
		assert(false);
		return GL_RGBA32F;
	}

	assert(false);
	return GL_RGBA8;
}

static GLenum convert_format(kore_gpu_texture_format format) {
	switch (format) {
	case KORE_GPU_TEXTURE_FORMAT_UNDEFINED:
		assert(false);
		return GL_RGBA;
	case KORE_GPU_TEXTURE_FORMAT_R8_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_R8_SNORM:
	case KORE_GPU_TEXTURE_FORMAT_R8_UINT:
	case KORE_GPU_TEXTURE_FORMAT_R8_SINT:
	case KORE_GPU_TEXTURE_FORMAT_R16_UINT:
	case KORE_GPU_TEXTURE_FORMAT_R16_SINT:
	case KORE_GPU_TEXTURE_FORMAT_R16_FLOAT:
		return GL_RED;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_RG8_SNORM:
	case KORE_GPU_TEXTURE_FORMAT_RG8_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RG8_SINT:
		return GL_RG;
	case KORE_GPU_TEXTURE_FORMAT_R32_UINT:
	case KORE_GPU_TEXTURE_FORMAT_R32_SINT:
	case KORE_GPU_TEXTURE_FORMAT_R32_FLOAT:
		return GL_RED;
	case KORE_GPU_TEXTURE_FORMAT_RG16_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RG16_SINT:
	case KORE_GPU_TEXTURE_FORMAT_RG16_FLOAT:
		return GL_RG;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM_SRGB:
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SNORM:
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SINT:
		return GL_RGBA;
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM_SRGB:
#ifdef GL_BGRA
		return GL_BGRA;
#else
		return GL_RGBA;
#endif
	case KORE_GPU_TEXTURE_FORMAT_RGB9E5U_FLOAT:
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_RG11B10U_FLOAT:
		assert(false);
		return GL_RGBA;
	case KORE_GPU_TEXTURE_FORMAT_RG32_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RG32_SINT:
	case KORE_GPU_TEXTURE_FORMAT_RG32_FLOAT:
		return GL_RG;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_SINT:
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_FLOAT:
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_UINT:
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_SINT:
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_FLOAT:
		return GL_RGBA;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH16_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24_NOTHING8:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24_STENCIL8:
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32_FLOAT:
		return GL_DEPTH_COMPONENT;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32_FLOAT_STENCIL8_NOTHING24:
		assert(false);
		return GL_RGBA;
	}

	assert(false);
	return GL_RGBA;
}

static GLenum texture_format_type(kore_gpu_texture_format format) {
	switch (format) {
	case KORE_GPU_TEXTURE_FORMAT_UNDEFINED:
		assert(false);
	case KORE_GPU_TEXTURE_FORMAT_R8_UNORM:
		return GL_UNSIGNED_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_R8_SNORM:
		return GL_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_R8_UINT:
		return GL_UNSIGNED_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_R8_SINT:
		return GL_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_R16_UINT:
		return GL_UNSIGNED_SHORT;
	case KORE_GPU_TEXTURE_FORMAT_R16_SINT:
		return GL_SHORT;
	case KORE_GPU_TEXTURE_FORMAT_R16_FLOAT:
		assert(false);
		return GL_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UNORM:
		return GL_UNSIGNED_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_RG8_SNORM:
		return GL_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_RG8_UINT:
		return GL_UNSIGNED_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_RG8_SINT:
		return GL_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_R32_UINT:
		return GL_UNSIGNED_INT;
	case KORE_GPU_TEXTURE_FORMAT_R32_SINT:
		return GL_INT;
	case KORE_GPU_TEXTURE_FORMAT_R32_FLOAT:
		return GL_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RG16_UINT:
		return GL_UNSIGNED_SHORT;
	case KORE_GPU_TEXTURE_FORMAT_RG16_SINT:
		return GL_SHORT;
	case KORE_GPU_TEXTURE_FORMAT_RG16_FLOAT:
		assert(false);
		return GL_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UNORM_SRGB:
		return GL_UNSIGNED_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SNORM:
		return GL_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_UINT:
		return GL_UNSIGNED_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_RGBA8_SINT:
		return GL_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM:
	case KORE_GPU_TEXTURE_FORMAT_BGRA8_UNORM_SRGB:
		return GL_UNSIGNED_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_RGB9E5U_FLOAT:
		assert(false);
		return GL_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UINT:
		assert(false);
		return GL_UNSIGNED_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_RGB10A2_UNORM:
		assert(false);
		return GL_UNSIGNED_BYTE;
	case KORE_GPU_TEXTURE_FORMAT_RG11B10U_FLOAT:
		assert(false);
		return GL_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RG32_UINT:
		return GL_UNSIGNED_INT;
	case KORE_GPU_TEXTURE_FORMAT_RG32_SINT:
		return GL_INT;
	case KORE_GPU_TEXTURE_FORMAT_RG32_FLOAT:
		return GL_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_UINT:
		return GL_UNSIGNED_SHORT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_SINT:
		return GL_SHORT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA16_FLOAT:
		assert(false);
		return GL_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_UINT:
		return GL_UNSIGNED_INT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_SINT:
		return GL_INT;
	case KORE_GPU_TEXTURE_FORMAT_RGBA32_FLOAT:
		return GL_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH16_UNORM:
		return GL_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24_NOTHING8:
		assert(false);
		return GL_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH24_STENCIL8:
		assert(false);
		return GL_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32_FLOAT:
		return GL_FLOAT;
	case KORE_GPU_TEXTURE_FORMAT_DEPTH32_FLOAT_STENCIL8_NOTHING24:
		assert(false);
		return GL_FLOAT;
	}

	assert(false);
	return GL_UNSIGNED_BYTE;
}

void kore_opengl_device_create_texture(kore_gpu_device *device, const kore_gpu_texture_parameters *parameters, kore_gpu_texture *texture) {
	uint32_t width  = parameters->width > 0 ? parameters->width : 1;
	uint32_t height = parameters->height > 0 ? parameters->height : 1;

	glGenTextures(1, &texture->opengl.texture);
	kore_opengl_check_errors();

	GLenum target = GL_TEXTURE_2D;
	if (parameters->depth_or_array_layers == 6) {
		target = GL_TEXTURE_CUBE_MAP;
	}
	else if (parameters->depth_or_array_layers > 1) {
		target = GL_TEXTURE_2D_ARRAY;
	}

	glBindTexture(target, texture->opengl.texture);
	kore_opengl_check_errors();

	GLenum internal_format = convert_internal_format(parameters->format);
	GLenum format          = convert_format(parameters->format);
	GLenum type            = texture_format_type(parameters->format);

	if (target == GL_TEXTURE_2D_ARRAY) {
		glTexStorage3D(target, parameters->mip_level_count, internal_format, width, height, parameters->depth_or_array_layers);
	}
	else {
		glTexStorage2D(target, parameters->mip_level_count, internal_format, width, height);
	}
	kore_opengl_check_errors();

	glBindTexture(target, 0);

	kore_opengl_check_errors();

	texture->width                  = width;
	texture->height                 = height;
	texture->opengl.format          = parameters->format;
	texture->opengl.target          = target;
	texture->opengl.mip_level_count = parameters->mip_level_count;
}

kore_gpu_texture *kore_opengl_device_get_framebuffer(kore_gpu_device *device) {
	uint32_t width  = kore_window_width(0);
	uint32_t height = kore_window_height(0);

	framebuffer.width  = width > 8 ? width : 8;
	framebuffer.height = height > 8 ? height : 8;

	return &framebuffer;
}

kore_gpu_texture_format kore_opengl_device_framebuffer_format(kore_gpu_device *device) {
	return framebuffer.opengl.format;
}

static uint32_t vertex_format_size(kore_opengl_vertex_format format) {
	switch (format) {
	case KORE_OPENGL_VERTEX_FORMAT_UINT8X2:
		return 2u;
	case KORE_OPENGL_VERTEX_FORMAT_UINT8X4:
		return 4u;
	case KORE_OPENGL_VERTEX_FORMAT_SINT8X2:
		return 2u;
	case KORE_OPENGL_VERTEX_FORMAT_SINT8X4:
		return 4u;
	case KORE_OPENGL_VERTEX_FORMAT_UNORM8X2:
		return 2u;
	case KORE_OPENGL_VERTEX_FORMAT_UNORM8X4:
		return 4u;
	case KORE_OPENGL_VERTEX_FORMAT_SNORM8X2:
		return 2u;
	case KORE_OPENGL_VERTEX_FORMAT_SNORM8X4:
		return 4u;
	case KORE_OPENGL_VERTEX_FORMAT_UINT16X2:
		return 2u;
	case KORE_OPENGL_VERTEX_FORMAT_UINT16X4:
		return 4u;
	case KORE_OPENGL_VERTEX_FORMAT_SINT16X2:
		return 2u;
	case KORE_OPENGL_VERTEX_FORMAT_SINT16X4:
		return 4u;
	case KORE_OPENGL_VERTEX_FORMAT_UNORM16X2:
		return 2u;
	case KORE_OPENGL_VERTEX_FORMAT_UNORM16X4:
		return 4u;
	case KORE_OPENGL_VERTEX_FORMAT_SNORM16X2:
		return 2u;
	case KORE_OPENGL_VERTEX_FORMAT_SNORM16X4:
		return 4u;
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT16X2:
		return 2u;
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT16X4:
		return 4u;
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT32:
		return 1u;
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT32X2:
		return 2u;
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT32X3:
		return 3u;
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT32X4:
		return 4u;
	case KORE_OPENGL_VERTEX_FORMAT_UINT32:
		return 1u;
	case KORE_OPENGL_VERTEX_FORMAT_UINT32X2:
		return 2u;
	case KORE_OPENGL_VERTEX_FORMAT_UINT32X3:
		return 3;
	case KORE_OPENGL_VERTEX_FORMAT_UINT32X4:
		return 4u;
	case KORE_OPENGL_VERTEX_FORMAT_SIN32:
		return 1u;
	case KORE_OPENGL_VERTEX_FORMAT_SINT32X2:
		return 2u;
	case KORE_OPENGL_VERTEX_FORMAT_SINT32X3:
		return 3u;
	case KORE_OPENGL_VERTEX_FORMAT_SINT32X4:
		return 4u;
	case KORE_OPENGL_VERTEX_FORMAT_UNORM10_10_10_2:
		return 4u;
	}

	assert(false);
	return 1u;
}

static GLenum vertex_format_type(kore_opengl_vertex_format format) {
	switch (format) {
	case KORE_OPENGL_VERTEX_FORMAT_UINT8X2:
	case KORE_OPENGL_VERTEX_FORMAT_UINT8X4:
		return GL_UNSIGNED_BYTE;
	case KORE_OPENGL_VERTEX_FORMAT_SINT8X2:
	case KORE_OPENGL_VERTEX_FORMAT_SINT8X4:
		return GL_BYTE;
	case KORE_OPENGL_VERTEX_FORMAT_UNORM8X2:
	case KORE_OPENGL_VERTEX_FORMAT_UNORM8X4:
		return GL_UNSIGNED_BYTE;
	case KORE_OPENGL_VERTEX_FORMAT_SNORM8X2:
	case KORE_OPENGL_VERTEX_FORMAT_SNORM8X4:
		return GL_BYTE;
	case KORE_OPENGL_VERTEX_FORMAT_UINT16X2:
	case KORE_OPENGL_VERTEX_FORMAT_UINT16X4:
		return GL_UNSIGNED_SHORT;
	case KORE_OPENGL_VERTEX_FORMAT_SINT16X2:
	case KORE_OPENGL_VERTEX_FORMAT_SINT16X4:
		return GL_SHORT;
	case KORE_OPENGL_VERTEX_FORMAT_UNORM16X2:
	case KORE_OPENGL_VERTEX_FORMAT_UNORM16X4:
		return GL_UNSIGNED_SHORT;
	case KORE_OPENGL_VERTEX_FORMAT_SNORM16X2:
	case KORE_OPENGL_VERTEX_FORMAT_SNORM16X4:
		return GL_SHORT;
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT16X2:
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT16X4:
		assert(false);
		return GL_FLOAT;
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT32:
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT32X2:
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT32X3:
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT32X4:
		return GL_FLOAT;
	case KORE_OPENGL_VERTEX_FORMAT_UINT32:
	case KORE_OPENGL_VERTEX_FORMAT_UINT32X2:
	case KORE_OPENGL_VERTEX_FORMAT_UINT32X3:
	case KORE_OPENGL_VERTEX_FORMAT_UINT32X4:
		return GL_UNSIGNED_INT;
	case KORE_OPENGL_VERTEX_FORMAT_SIN32:
	case KORE_OPENGL_VERTEX_FORMAT_SINT32X2:
	case KORE_OPENGL_VERTEX_FORMAT_SINT32X3:
	case KORE_OPENGL_VERTEX_FORMAT_SINT32X4:
		return GL_INT;
	case KORE_OPENGL_VERTEX_FORMAT_UNORM10_10_10_2:
		assert(false);
		return GL_INT;
	}

	assert(false);
	return 1u;
}

static bool vertex_format_normalized(kore_opengl_vertex_format format) {
	switch (format) {
	case KORE_OPENGL_VERTEX_FORMAT_UINT8X2:
	case KORE_OPENGL_VERTEX_FORMAT_UINT8X4:
	case KORE_OPENGL_VERTEX_FORMAT_SINT8X2:
	case KORE_OPENGL_VERTEX_FORMAT_SINT8X4:
		return false;
	case KORE_OPENGL_VERTEX_FORMAT_UNORM8X2:
	case KORE_OPENGL_VERTEX_FORMAT_UNORM8X4:
	case KORE_OPENGL_VERTEX_FORMAT_SNORM8X2:
	case KORE_OPENGL_VERTEX_FORMAT_SNORM8X4:
		return true;
	case KORE_OPENGL_VERTEX_FORMAT_UINT16X2:
	case KORE_OPENGL_VERTEX_FORMAT_UINT16X4:
	case KORE_OPENGL_VERTEX_FORMAT_SINT16X2:
	case KORE_OPENGL_VERTEX_FORMAT_SINT16X4:
		return false;
	case KORE_OPENGL_VERTEX_FORMAT_UNORM16X2:
	case KORE_OPENGL_VERTEX_FORMAT_UNORM16X4:
	case KORE_OPENGL_VERTEX_FORMAT_SNORM16X2:
	case KORE_OPENGL_VERTEX_FORMAT_SNORM16X4:
		return true;
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT16X2:
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT16X4:
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT32:
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT32X2:
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT32X3:
	case KORE_OPENGL_VERTEX_FORMAT_FLOAT32X4:
	case KORE_OPENGL_VERTEX_FORMAT_UINT32:
	case KORE_OPENGL_VERTEX_FORMAT_UINT32X2:
	case KORE_OPENGL_VERTEX_FORMAT_UINT32X3:
	case KORE_OPENGL_VERTEX_FORMAT_UINT32X4:
	case KORE_OPENGL_VERTEX_FORMAT_SIN32:
	case KORE_OPENGL_VERTEX_FORMAT_SINT32X2:
	case KORE_OPENGL_VERTEX_FORMAT_SINT32X3:
	case KORE_OPENGL_VERTEX_FORMAT_SINT32X4:
		return false;
	case KORE_OPENGL_VERTEX_FORMAT_UNORM10_10_10_2:
		return true;
	}

	assert(false);
	return 1u;
}

static const uint32_t max_vertex_attrib_arrays = 16;

void kore_opengl_device_execute_command_list(kore_gpu_device *device, kore_gpu_command_list *list) {
	kore_gpu_index_format        index_format                    = KORE_GPU_INDEX_FORMAT_UINT16;
	bool                         primary_framebuffer             = false;
	bool                         written_to_internal_framebuffer = false;
	kore_opengl_render_pipeline *pipeline                        = NULL;
	set_texture                 *texture                         = NULL;
	set_sampler                 *sampler                         = NULL;

#define MAX_VERTEX_BUFFERS 8
	set_vertex_buffer_data *vertex_buffers[MAX_VERTEX_BUFFERS]    = {0};
	bool                    vertex_buffer_set[MAX_VERTEX_BUFFERS] = {0};

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

			assert(data->slot < MAX_VERTEX_BUFFERS);
			vertex_buffers[data->slot]    = data;
			vertex_buffer_set[data->slot] = true;

			break;
		}
		case COMMAND_DRAW_INDEXED: {
			if (texture != NULL) {
				glActiveTexture(GL_TEXTURE0);

				glBindTexture(texture->view.texture->opengl.target, texture->view.texture->opengl.texture);

				kore_opengl_check_errors();

				if (sampler != NULL) {
					if (texture->view.texture->opengl.mip_level_count > 1) {
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler->sampler.opengl.mip_filter);
						glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, sampler->sampler.opengl.mip_filter);
					}
					else {
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler->sampler.opengl.min_filter);
						glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, sampler->sampler.opengl.min_filter);
					}

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler->sampler.opengl.mag_filter);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, sampler->sampler.opengl.mag_filter);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler->sampler.opengl.address_mode_u);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler->sampler.opengl.address_mode_v);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, sampler->sampler.opengl.address_mode_w);

					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, sampler->sampler.opengl.address_mode_u);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, sampler->sampler.opengl.address_mode_v);
					glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, sampler->sampler.opengl.address_mode_w);

					kore_opengl_check_errors();
				}
			}

			kore_opengl_vertex_state *vertex_state = &pipeline->vertex_state;

			uint32_t input_index = 0;

			for (uint32_t buffer_index = 0; buffer_index < MAX_VERTEX_BUFFERS; ++buffer_index) {
				if (vertex_buffer_set[buffer_index]) {
					set_vertex_buffer_data *buffer_data = vertex_buffers[buffer_index];
					glBindBuffer(buffer_data->buffer->buffer_type, buffer_data->buffer->buffer);

					for (uint32_t attribute_index = 0; attribute_index < vertex_state->buffers[buffer_index].attributes_count; ++attribute_index) {
						kore_opengl_vertex_attribute *attribute = &vertex_state->buffers[buffer_index].attributes[attribute_index];

						glEnableVertexAttribArray(input_index);

						glVertexAttribPointer(input_index, vertex_format_size(attribute->format), vertex_format_type(attribute->format),
						                      vertex_format_normalized(attribute->format), (GLsizei)vertex_state->buffers[buffer_index].array_stride,
						                      (void *)(int64_t)attribute->offset);

						glVertexAttribDivisor(input_index, vertex_state->buffers[buffer_index].step_mode == KORE_OPENGL_VERTEX_STEP_MODE_INSTANCE ? 1 : 0);

						kore_opengl_check_errors();

						++input_index;
					}
				}
			}

			for (; input_index < max_vertex_attrib_arrays; ++input_index) {
				glDisableVertexAttribArray(input_index);
			}

			draw_indexed_data *data = (draw_indexed_data *)&c->data;

			void *start =
			    index_format == KORE_GPU_INDEX_FORMAT_UINT16 ? (void *)(data->first_index * sizeof(uint16_t)) : (void *)(data->first_index * sizeof(uint32_t));
			glDrawElementsInstanced(GL_TRIANGLES, data->index_count, index_format == KORE_GPU_INDEX_FORMAT_UINT16 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, start,
			                        data->instance_count);

			memset(vertex_buffer_set, 0, sizeof(vertex_buffer_set));

			break;
		}
		case COMMAND_SET_RENDER_PIPELINE: {
			set_render_pipeline *data = (set_render_pipeline *)&c->data;

			if (primary_framebuffer) {
				glUseProgram(data->pipeline->program);
			}
			else {
				glUseProgram(data->pipeline->flip_program);
			}
			kore_opengl_check_errors();

			switch (data->pipeline->depth_stencil.depth_compare) {
			case KORE_GPU_COMPARE_FUNCTION_UNDEFINED:
				break;
			case KORE_GPU_COMPARE_FUNCTION_NEVER:
				glDepthFunc(GL_NEVER);
				break;
			case KORE_GPU_COMPARE_FUNCTION_LESS:
				glDepthFunc(GL_LESS);
				break;
			case KORE_GPU_COMPARE_FUNCTION_EQUAL:
				glDepthFunc(GL_EQUAL);
				break;
			case KORE_GPU_COMPARE_FUNCTION_LESS_EQUAL:
				glDepthFunc(GL_LEQUAL);
				break;
			case KORE_GPU_COMPARE_FUNCTION_GREATER:
				glDepthFunc(GL_GREATER);
				break;
			case KORE_GPU_COMPARE_FUNCTION_NOT_EQUAL:
				glDepthFunc(GL_NOTEQUAL);
				break;
			case KORE_GPU_COMPARE_FUNCTION_GREATER_EQUAL:
				glDepthFunc(GL_GEQUAL);
				break;
			case KORE_GPU_COMPARE_FUNCTION_ALWAYS:
				glDepthFunc(GL_ALWAYS);
				break;
			}

			pipeline = data->pipeline;

			break;
		}
		case COMMAND_SET_UNIFORM_BUFFER: {
			set_uniform_buffer *data = (set_uniform_buffer *)&c->data;

			glBindBufferRange(GL_UNIFORM_BUFFER, data->buffer->opengl.uniform_buffer, data->buffer->opengl.buffer, data->offset, data->size);

			glUniformBlockBinding(pipeline->program, data->uniform_block_index, data->buffer->opengl.uniform_buffer);
			kore_opengl_check_errors();

			break;
		}
		case COMMAND_SET_TEXTURE:
			texture = (set_texture *)&c->data;
			break;
		case COMMAND_SET_SAMPLER:
			sampler = (set_sampler *)&c->data;
			break;
		case COMMAND_COPY_BUFFER_TO_TEXTURE: {
			copy_buffer_to_texture *data = (copy_buffer_to_texture *)&c->data;

			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, data->source.buffer->opengl.buffer);

			glActiveTexture(GL_TEXTURE0);

			glBindTexture(data->destination.texture->opengl.target, data->destination.texture->opengl.texture);

			GLenum internal_format = convert_internal_format(data->destination.texture->opengl.format);
			GLenum format          = convert_format(data->destination.texture->opengl.format);
			GLenum type            = texture_format_type(data->destination.texture->opengl.format);

			if (data->destination.texture->opengl.target == GL_TEXTURE_2D_ARRAY) {
				glTexSubImage3D(data->destination.texture->opengl.target, data->destination.mip_level, 0, 0, data->destination.origin_z, data->width,
				                data->height, data->depth_or_array_layers, format, type, 0);
			}
			else {
				glTexSubImage2D(data->destination.texture->opengl.target, data->destination.mip_level, 0, 0, data->width, data->height, format, type, 0);
			}

			glBindTexture(data->destination.texture->opengl.target, 0);

			kore_opengl_check_errors();

			break;
		}
		case COMMAND_COPY_TEXTURE_TO_BUFFER: {
			copy_texture_to_buffer *data = (copy_texture_to_buffer *)&c->data;

			if (data->source.texture->opengl.is_primary_framebuffer) {
				// read framebuffer into the buffer
				glBindBuffer(data->destination.buffer->opengl.buffer_type, data->destination.buffer->opengl.buffer);
				glReadPixels(0, 0, data->width, data->height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glBindBuffer(data->destination.buffer->opengl.buffer_type, 0);

				kore_opengl_check_errors();

				// create texture backed by the buffer
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, data->destination.buffer->opengl.buffer);
				uint32_t texture;
				glGenTextures(1, &texture);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data->width, data->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
				glBindTexture(GL_TEXTURE_2D, 0);

				kore_opengl_check_errors();

				uint32_t flipped_framebuffer = flip(data->width, data->height, texture);

				// read the flipped framebuffer into the buffer
				glBindFramebuffer(GL_FRAMEBUFFER, flipped_framebuffer);
				glBindBuffer(data->destination.buffer->opengl.buffer_type, data->destination.buffer->opengl.buffer);
				glReadPixels(0, 0, data->width, data->height, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glBindBuffer(data->destination.buffer->opengl.buffer_type, 0);

				kore_opengl_check_errors();
			}
			else {
				assert(false);
			}

			break;
		}
		case COMMAND_COPY_TEXTURE_TO_TEXTURE: {
			copy_texture_to_texture *data = (copy_texture_to_texture *)&c->data;

			if (data->destination.texture->opengl.is_primary_framebuffer) {
				copy(data->source.origin_x, data->source.origin_y, data->source.texture->width, data->source.texture->height, data->destination.origin_x,
				     data->destination.origin_y, data->destination.texture->width, data->destination.texture->height, data->width, data->height,
				     data->source.texture->opengl.texture, device->opengl.default_framebuffer);
			}
			else {
				glBindFramebuffer(GL_FRAMEBUFFER, device->opengl.custom_framebuffer);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, data->source.texture->opengl.texture, 0);

				glReadBuffer(GL_COLOR_ATTACHMENT0);
				glBindTexture(GL_TEXTURE_2D, data->destination.texture->opengl.texture);
				glCopyTexSubImage2D(GL_TEXTURE_2D, 0, data->destination.origin_x, data->destination.origin_y, data->source.origin_x, data->source.origin_y,
				                    data->width, data->height);
			}

			kore_opengl_check_errors();

			break;
		}
		case COMMAND_BEGIN_RENDER_PASS: {
			begin_render_pass *data = (begin_render_pass *)&c->data;

			written_to_internal_framebuffer = false;

			if (data->parameters.color_attachments_count == 1 && data->parameters.color_attachments[0].texture.texture->opengl.is_primary_framebuffer &&
			    data->parameters.depth_stencil_attachment.texture == NULL) {
				primary_framebuffer = true;

				glBindFramebuffer(GL_FRAMEBUFFER, device->opengl.default_framebuffer);
				kore_opengl_check_errors();

				glViewport(0, 0, data->parameters.color_attachments[0].texture.texture->width, data->parameters.color_attachments[0].texture.texture->height);
				kore_opengl_check_errors();

				if (data->parameters.color_attachments[0].load_op == KORE_GPU_LOAD_OP_CLEAR) {
					kore_gpu_color color = data->parameters.color_attachments[0].clear_value;
					glClearColor(color.r, color.g, color.b, color.a);
					glClear(GL_COLOR_BUFFER_BIT);
					kore_opengl_check_errors();
				}
			}
			else {
				primary_framebuffer = false;

				glBindFramebuffer(GL_FRAMEBUFFER, device->opengl.custom_framebuffer);
				kore_opengl_check_errors();

				for (size_t color_index = 0; color_index < data->parameters.color_attachments_count; ++color_index) {
					kore_gpu_texture_view *view = &data->parameters.color_attachments[color_index].texture;

					if (view->texture->opengl.is_primary_framebuffer) {
						written_to_internal_framebuffer = true;
					}

					if (color_index == 0) {
						glViewport(0, 0, view->texture->width, view->texture->height);
					}

					if (view->texture->opengl.target == GL_TEXTURE_CUBE_MAP) {
						glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)(GL_COLOR_ATTACHMENT0 + color_index),
						                       GL_TEXTURE_CUBE_MAP_POSITIVE_X + view->base_array_layer, view->texture->opengl.texture, 0);
					}
					else {
						glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)(GL_COLOR_ATTACHMENT0 + color_index), GL_TEXTURE_2D, view->texture->opengl.texture, 0);
					}

					if (data->parameters.color_attachments[color_index].load_op == KORE_GPU_LOAD_OP_CLEAR) {
						kore_gpu_color color = data->parameters.color_attachments[0].clear_value;
						glClearBufferfv(GL_COLOR, (GLint)color_index, (float *)&color);
						kore_opengl_check_errors();
					}
				}

				GLenum draw_buffers[16];
				for (size_t color_index = 0; color_index < data->parameters.color_attachments_count; ++color_index) {
					draw_buffers[color_index] = (GLenum)(GL_COLOR_ATTACHMENT0 + color_index);
				}
				glDrawBuffers((GLsizei)data->parameters.color_attachments_count, draw_buffers);

				for (size_t color_index = data->parameters.color_attachments_count; color_index < 8; ++color_index) {
					glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)(GL_COLOR_ATTACHMENT0 + color_index), GL_TEXTURE_2D, 0, 0);
				}

				if (data->parameters.depth_stencil_attachment.texture != NULL) {
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
					                       data->parameters.depth_stencil_attachment.texture->opengl.texture, 0);

					glEnable(GL_DEPTH_TEST);

					if (data->parameters.depth_stencil_attachment.depth_load_op == KORE_GPU_LOAD_OP_CLEAR) {
						glClearBufferfv(GL_DEPTH, 0, &data->parameters.depth_stencil_attachment.depth_clear_value);
					}

					if (data->parameters.depth_stencil_attachment.stencil_load_op == KORE_GPU_LOAD_OP_CLEAR) {
						glClearBufferiv(GL_STENCIL, 0, &data->parameters.depth_stencil_attachment.stencil_clear_value);
					}
				}
				else {
					glDisable(GL_DEPTH_TEST);
				}
			}

			break;
		}
		case COMMAND_END_RENDER_PASS: {
			if (written_to_internal_framebuffer) {
				glBindFramebuffer(GL_READ_FRAMEBUFFER, device->opengl.custom_framebuffer);
				glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer.opengl.texture, 0);

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, device->opengl.default_framebuffer);

				glBlitFramebuffer(0, 0, framebuffer.width, framebuffer.height, 0, framebuffer.height, framebuffer.width, 0, GL_COLOR_BUFFER_BIT,
				                  GL_NEAREST); // flip
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			kore_opengl_check_errors();

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

static unsigned convert_address_mode(kore_gpu_address_mode mode) {
	switch (mode) {
	case KORE_GPU_ADDRESS_MODE_CLAMP_TO_EDGE:
		return GL_CLAMP_TO_EDGE;
	case KORE_GPU_ADDRESS_MODE_REPEAT:
		return GL_REPEAT;
	case KORE_GPU_ADDRESS_MODE_MIRROR_REPEAT:
		return GL_MIRRORED_REPEAT;
	}

	assert(false);
	return KORE_GPU_ADDRESS_MODE_REPEAT;
}

static unsigned convert_filter_mode(kore_gpu_filter_mode mode) {
	switch (mode) {
	case KORE_GPU_FILTER_MODE_NEAREST:
		return GL_NEAREST;
	case KORE_GPU_FILTER_MODE_LINEAR:
		return GL_LINEAR;
	}

	assert(false);
	return GL_NEAREST;
}

static unsigned convert_mipmap_filter_mode(kore_gpu_filter_mode mode, kore_gpu_mipmap_filter_mode mipmap_mode) {
	switch (mode) {
	case KORE_GPU_FILTER_MODE_NEAREST:
		switch (mipmap_mode) {
		case KORE_GPU_MIPMAP_FILTER_MODE_NEAREST:
			return GL_NEAREST_MIPMAP_NEAREST;
		case KORE_GPU_MIPMAP_FILTER_MODE_LINEAR:
			return GL_NEAREST_MIPMAP_LINEAR;
		}
		break;
	case KORE_GPU_FILTER_MODE_LINEAR:
		switch (mipmap_mode) {
		case KORE_GPU_MIPMAP_FILTER_MODE_NEAREST:
			return GL_LINEAR_MIPMAP_NEAREST;
		case KORE_GPU_MIPMAP_FILTER_MODE_LINEAR:
			return GL_LINEAR_MIPMAP_LINEAR;
		}
		break;
	}

	assert(false);
	return GL_NEAREST;
}

static unsigned convert_compare_function(kore_gpu_compare_function func) {
	switch (func) {
	case KORE_GPU_COMPARE_FUNCTION_UNDEFINED:
	case KORE_GPU_COMPARE_FUNCTION_NEVER:
		return GL_NEVER;
	case KORE_GPU_COMPARE_FUNCTION_LESS:
		return GL_LESS;
	case KORE_GPU_COMPARE_FUNCTION_EQUAL:
		return GL_EQUAL;
	case KORE_GPU_COMPARE_FUNCTION_LESS_EQUAL:
		return GL_LEQUAL;
	case KORE_GPU_COMPARE_FUNCTION_GREATER:
		return GL_GREATER;
	case KORE_GPU_COMPARE_FUNCTION_NOT_EQUAL:
		return GL_NOTEQUAL;
	case KORE_GPU_COMPARE_FUNCTION_GREATER_EQUAL:
		return GL_GEQUAL;
	case KORE_GPU_COMPARE_FUNCTION_ALWAYS:
		return GL_ALWAYS;
	}

	assert(false);
	return GL_NEVER;
}

void kore_opengl_device_create_sampler(kore_gpu_device *device, const kore_gpu_sampler_parameters *parameters, kore_gpu_sampler *sampler) {
	sampler->opengl.address_mode_u = convert_address_mode(parameters->address_mode_u);
	sampler->opengl.address_mode_v = convert_address_mode(parameters->address_mode_v);
	sampler->opengl.address_mode_w = convert_address_mode(parameters->address_mode_w);

	sampler->opengl.min_filter = convert_filter_mode(parameters->min_filter);
	sampler->opengl.mag_filter = convert_filter_mode(parameters->mag_filter);

	sampler->opengl.mip_filter = convert_mipmap_filter_mode(parameters->min_filter, parameters->mipmap_filter);

	sampler->opengl.lod_min_clamp = parameters->lod_min_clamp;
	sampler->opengl.lod_max_clamp = parameters->lod_max_clamp;

	sampler->opengl.compare = convert_compare_function(parameters->compare);

	sampler->opengl.max_anisotropy = parameters->max_anisotropy;
}

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
