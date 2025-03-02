#include <kore3/opengl/texture_functions.h>

#include "openglunit.h"

#include <kore3/gpu/texture.h>

static const char *flip_vertex_shader_source = "#version 330\n"
                                               "layout(location = 0) in vec2 pos;"
                                               "layout(location = 1) in vec2 tex;"
                                               "out vec2 texcoord;"
                                               "void main() {"
                                               "gl_Position = vec4(pos.x, pos.y, 0.5, 1.0);"
                                               "texcoord = tex;"
                                               "}";

static const char *flip_fragment_shader_source = "#version 330\n"
                                                 "out vec4 color;"
                                                 "uniform sampler2D tex;"
                                                 "in vec2 texcoord;"
                                                 "void main() {"
                                                 "color = texture(tex, texcoord);"
                                                 "}";

static uint32_t flip_vertex_shader;
static uint32_t flip_fragment_shader;
static uint32_t flip_program;
static uint32_t flip_texture;
static uint32_t flip_framebuffer;
static uint32_t flip_vertex_buffer;
static uint32_t flip_index_buffer;
static uint32_t flip_width = 0;
static uint32_t flip_height = 0;

static void init_flip(void) {
	flip_program = glCreateProgram();
	kore_opengl_check_errors();

	flip_vertex_shader = compile_shader(GL_VERTEX_SHADER, flip_vertex_shader_source);
	flip_fragment_shader = compile_shader(GL_FRAGMENT_SHADER, flip_fragment_shader_source);

	glAttachShader(flip_program, flip_vertex_shader);
	glAttachShader(flip_program, flip_fragment_shader);
	kore_opengl_check_errors();

	glBindAttribLocation(flip_program, 0, "pos");
	glBindAttribLocation(flip_program, 1, "tex");
	kore_opengl_check_errors();

	link_program(flip_program);

	{
		glGenBuffers(1, &flip_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, flip_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, 4 * 2 * 4, NULL, GL_DYNAMIC_DRAW);
		float *data = (float *)glMapBufferRange(GL_ARRAY_BUFFER, 0, 3 * 2 * 4, GL_MAP_WRITE_BIT);
		// clang-format off
		data[0] = -1.0f; data[1] =  1.0f; data[ 2] = 0.0f; data[ 3] = 0.0f;
		data[4] =  2.0f; data[5] =  1.0f; data[ 6] = 2.0f; data[ 7] = 0.0f;
		data[8] = -1.0f; data[9] = -2.0f; data[10] = 0.0f; data[11] = 2.0f;
		// clang-format on
		glUnmapBuffer(GL_ARRAY_BUFFER);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	{
		glGenBuffers(1, &flip_index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, flip_vertex_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * 2, NULL, GL_DYNAMIC_DRAW);
		uint16_t *data = (uint16_t *)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, 3 * 2, GL_MAP_WRITE_BIT);
		data[0] = 0;
		data[1] = 1;
		data[2] = 2;
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

static void flip(uint32_t width, uint32_t height, uint32_t source_texture) {
	if (width != flip_width || height != flip_height) {
		glGenTextures(1, &flip_texture);
		glBindTexture(GL_TEXTURE_2D, flip_texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_POINT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_POINT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glGenFramebuffers(1, &flip_framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, flip_framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, flip_texture, 0);
		kore_opengl_check_errors();

		glBindTexture(GL_TEXTURE_2D, 0);

		flip_width = width;
		flip_height = height;
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, flip_framebuffer);
	}

	glBindBuffer(GL_ARRAY_BUFFER, flip_vertex_buffer);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * 4, (void *)(int64_t)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 4 * 4, (void *)(int64_t)(2 * 4));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, flip_index_buffer);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, source_texture);

	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);

	kore_opengl_check_errors();
}
