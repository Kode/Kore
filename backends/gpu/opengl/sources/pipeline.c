#include <kore3/opengl/pipeline_functions.h>
#include <kore3/opengl/pipeline_structs.h>

#include <kore3/log.h>

#define COMPILE_ERROR_MESSAGE_MAX_LENGTH 1024
static char compile_error_message[COMPILE_ERROR_MESSAGE_MAX_LENGTH];

static uint32_t compile_shader(uint32_t shader_type, const char *source) {
	uint32_t shader = glCreateShader(shader_type);
	kore_opengl_check_errors();

	glShaderSource(shader, 1, (const GLchar **)&source, 0);
	glCompileShader(shader);

	int result = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result != GL_TRUE) {
		glGetShaderInfoLog(shader, COMPILE_ERROR_MESSAGE_MAX_LENGTH, NULL, compile_error_message);
		kore_log(KORE_LOG_LEVEL_ERROR, "GLSL compile error: %s", compile_error_message);
	}

	return shader;
}

void kore_opengl_render_pipeline_init(kore_opengl_device *device, kore_opengl_render_pipeline *pipe, const kore_opengl_render_pipeline_parameters *parameters) {
	pipe->program = glCreateProgram();
	kore_opengl_check_errors();

	pipe->vertex_shader = compile_shader(GL_VERTEX_SHADER, parameters->vertex.shader.data);
	pipe->fragment_shader = compile_shader(GL_FRAGMENT_SHADER, parameters->fragment.shader.data);

	glAttachShader(pipe->program, pipe->vertex_shader);
	glAttachShader(pipe->program, pipe->fragment_shader);
	kore_opengl_check_errors();

	for (size_t buffer_index = 0; buffer_index < parameters->vertex.buffers_count; ++buffer_index) {
		for (size_t attribute_index = 0; attribute_index < parameters->vertex.buffers[buffer_index].attributes_count; ++attribute_index) {
			const kore_opengl_vertex_attribute *attribute = &parameters->vertex.buffers[buffer_index].attributes[attribute_index];
			glBindAttribLocation(pipe->program, (GLuint)attribute_index, attribute->name);
			kore_opengl_check_errors();
		}
	}

	glLinkProgram(pipe->program);

	int result = GL_FALSE;
	glGetProgramiv(pipe->program, GL_LINK_STATUS, &result);
	if (result != GL_TRUE) {
		glGetProgramInfoLog(pipe->program, COMPILE_ERROR_MESSAGE_MAX_LENGTH, NULL, compile_error_message);
		kore_log(KORE_LOG_LEVEL_ERROR, "GLSL link error: %s", compile_error_message);
	}
}

void kore_opengl_render_pipeline_destroy(kore_opengl_render_pipeline *pipe) {}

void kore_opengl_compute_pipeline_init(kore_opengl_device *device, kore_opengl_compute_pipeline *pipe,
                                       const kore_opengl_compute_pipeline_parameters *parameters) {}

void kore_opengl_compute_pipeline_destroy(kore_opengl_compute_pipeline *pipe) {}

void kore_opengl_ray_pipeline_init(kore_gpu_device *device, kore_opengl_ray_pipeline *pipe, const kore_opengl_ray_pipeline_parameters *parameters) {}

void kore_opengl_ray_pipeline_destroy(kore_opengl_ray_pipeline *pipe) {}
