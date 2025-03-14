#ifndef KORE_OPENGL_UNIT_HEADER
#define KORE_OPENGL_UNIT_HEADER

#include <kore3/global.h>

#ifdef KORE_WINDOWS
#include <GL/glew.h>

#include <GL/gl.h>
#endif

#ifdef KORE_MACOS
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>
#endif

#ifdef KORE_IOS
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <OpenGLES/ES3/gl.h>
#endif

#ifdef KORE_ANDROID
#include <EGL/egl.h>
#if KORE_ANDROID_API >= 18
#include <GLES3/gl3.h>
#endif
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

#ifdef KORE_EMSCRIPTEN
#define GL_GLEXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#ifdef KORE_LINUX
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#ifdef KORE_RASPBERRY_PI
// #define GL_GLEXT_PROTOTYPES
#include "GLES2/gl2.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"
#endif

#ifdef KORE_WASM
#include <GL/gl.h>
#endif

#ifdef NDEBUG
static inline void kore_opengl_check_errors(void) {}
#else
void kore_opengl_check_errors(void);
#endif

static void     init_flip(void);
static uint32_t flip(uint32_t width, uint32_t height, uint32_t source_texture);
static uint32_t compile_shader(uint32_t shader_type, const char *source);
static void     link_program(uint32_t program);

#endif
