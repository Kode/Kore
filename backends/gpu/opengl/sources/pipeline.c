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

static void link_program(uint32_t program) {
	glLinkProgram(program);

	int result = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if (result != GL_TRUE) {
		glGetProgramInfoLog(program, COMPILE_ERROR_MESSAGE_MAX_LENGTH, NULL, compile_error_message);
		kore_log(KORE_LOG_LEVEL_ERROR, "GLSL link error: %s", compile_error_message);
	}
}

static void bind_attributes(const kore_opengl_vertex_state *vertex_state, uint32_t program) {
	size_t input_index = 0;

	for (size_t buffer_index = 0; buffer_index < vertex_state->buffers_count; ++buffer_index) {
		for (size_t attribute_index = 0; attribute_index < vertex_state->buffers[buffer_index].attributes_count; ++attribute_index) {
			const kore_opengl_vertex_attribute *attribute = &vertex_state->buffers[buffer_index].attributes[attribute_index];
			glBindAttribLocation(program, (GLuint)input_index, attribute->name);
			kore_opengl_check_errors();

			++input_index;
		}
	}
}

void kore_opengl_render_pipeline_init(kore_opengl_device *device, kore_opengl_render_pipeline *pipe, const kore_opengl_render_pipeline_parameters *parameters) {
	pipe->program = glCreateProgram();
	kore_opengl_check_errors();

	pipe->vertex_shader   = compile_shader(GL_VERTEX_SHADER, parameters->vertex.shader.data);
	pipe->fragment_shader = compile_shader(GL_FRAGMENT_SHADER, parameters->fragment.shader.data);

	glAttachShader(pipe->program, pipe->vertex_shader);
	glAttachShader(pipe->program, pipe->fragment_shader);
	kore_opengl_check_errors();

	bind_attributes(&parameters->vertex, pipe->program);
	pipe->vertex_state = parameters->vertex;

	pipe->depth_stencil = parameters->depth_stencil;

	link_program(pipe->program);

	pipe->flip_program = glCreateProgram();
	kore_opengl_check_errors();

	pipe->flip_vertex_shader = compile_shader(GL_VERTEX_SHADER, parameters->vertex.shader.flip_data);

	glAttachShader(pipe->flip_program, pipe->flip_vertex_shader);
	glAttachShader(pipe->flip_program, pipe->fragment_shader);
	kore_opengl_check_errors();

	bind_attributes(&parameters->vertex, pipe->flip_program);

	link_program(pipe->flip_program);
}

void kore_opengl_render_pipeline_destroy(kore_opengl_render_pipeline *pipe) {}

void kore_opengl_compute_pipeline_init(kore_opengl_device *device, kore_opengl_compute_pipeline *pipe,
                                       const kore_opengl_compute_pipeline_parameters *parameters) {}

void kore_opengl_compute_pipeline_destroy(kore_opengl_compute_pipeline *pipe) {}

void kore_opengl_ray_pipeline_init(kore_gpu_device *device, kore_opengl_ray_pipeline *pipe, const kore_opengl_ray_pipeline_parameters *parameters) {}

void kore_opengl_ray_pipeline_destroy(kore_opengl_ray_pipeline *pipe) {}
