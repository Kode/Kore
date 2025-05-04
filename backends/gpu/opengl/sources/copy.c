#include <kore3/opengl/texture_functions.h>

#include "openglunit.h"

#include <kore3/gpu/texture.h>

static const char *copy_vertex_shader_source = "#version 330\n"
                                               "layout(location = 0) in vec2 pos;"
                                               "layout(location = 1) in vec2 tex;"
                                               "out vec2 texcoord;"
                                               "void main() {"
                                               "gl_Position = vec4(pos.x, -pos.y, 0.5, 1.0);"
                                               "texcoord = tex;"
                                               "}";

static const char *copy_fragment_shader_source = "#version 330\n"
                                                 "out vec4 color;"
                                                 "uniform sampler2D tex;"
                                                 "in vec2 texcoord;"
                                                 "void main() {"
                                                 "color = texture(tex, texcoord);"
                                                 "}";

static uint32_t copy_vertex_shader;
static uint32_t copy_fragment_shader;
static uint32_t copy_program;
static uint32_t copy_vertex_buffer;
static uint32_t copy_index_buffer;

static void init_copy(void) {
	copy_program = glCreateProgram();
	kore_opengl_check_errors();

	copy_vertex_shader   = compile_shader(GL_VERTEX_SHADER, copy_vertex_shader_source);
	copy_fragment_shader = compile_shader(GL_FRAGMENT_SHADER, copy_fragment_shader_source);

	glAttachShader(copy_program, copy_vertex_shader);
	glAttachShader(copy_program, copy_fragment_shader);
	kore_opengl_check_errors();

	glBindAttribLocation(copy_program, 0, "pos");
	glBindAttribLocation(copy_program, 1, "tex");
	kore_opengl_check_errors();

	link_program(copy_program);

	{
		glGenBuffers(1, &copy_vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, copy_vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, 4 * 4 * 4, NULL, GL_DYNAMIC_DRAW);
#ifdef KORE_WEBGL
		float data[4 * 4];
#else
		float *data = (float *)glMapBufferRange(GL_ARRAY_BUFFER, 0, 4 * 4 * 4, GL_MAP_WRITE_BIT);
#endif
		// clang-format off
		data[ 0] = -1.0f; data[ 1] = -1.0f; data[ 2] = 0.0f; data[ 3] = 0.0f;
		data[ 4] = -1.0f; data[ 5] =  1.0f; data[ 6] = 0.0f; data[ 7] = 1.0f;
		data[ 8] =  1.0f; data[ 9] =  1.0f; data[10] = 1.0f; data[11] = 1.0f;
		data[12] =  1.0f; data[13] = -1.0f; data[14] = 1.0f; data[15] = 0.0f;
		// clang-format on
#ifdef KORE_WEBGL
		glBufferData(GL_ARRAY_BUFFER, 4 * 4 * 4, data, GL_STATIC_DRAW);
#else
		glUnmapBuffer(GL_ARRAY_BUFFER);
#endif
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	{
		glGenBuffers(1, &copy_index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, copy_index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 2, NULL, GL_DYNAMIC_DRAW);
#ifdef KORE_WEBGL
		uint16_t data[6];
#else
		uint16_t *data = (uint16_t *)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, 6 * 2, GL_MAP_WRITE_BIT);
#endif
		data[0] = 0;
		data[1] = 1;
		data[2] = 2;
		data[3] = 0;
		data[4] = 2;
		data[5] = 3;
#ifdef KORE_WEBGL
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * 2, data, GL_STATIC_DRAW);
#else
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
#endif
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

static void copy(uint32_t source_x, uint32_t source_y, uint32_t source_w, uint32_t source_h, uint32_t destination_x, uint32_t destination_y,
                 uint32_t destination_w, uint32_t destination_h, uint32_t width, uint32_t height, uint32_t source_texture, uint32_t destination_framebuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, destination_framebuffer);

	glViewport(0, 0, destination_w, destination_h);

	// do normal rendering into the render target
	glUseProgram(copy_program);

	glBindBuffer(GL_ARRAY_BUFFER, copy_vertex_buffer);

	{
		glBufferData(GL_ARRAY_BUFFER, 4 * 4 * 4, NULL, GL_DYNAMIC_DRAW);
#ifdef KORE_WEBGL
		float data[4 * 4];
#else
		float *data = (float *)glMapBufferRange(GL_ARRAY_BUFFER, 0, 4 * 4 * 4, GL_MAP_WRITE_BIT);
#endif
		float left_pos   = (destination_x / (float)destination_w) * 2.0f - 1.0f;
		float right_pos  = ((destination_x + width) / (float)destination_w) * 2.0f - 1.0f;
		float top_pos    = ((destination_y + height) / (float)destination_h) * 2.0f - 1.0f;
		float bottom_pos = (destination_y / (float)destination_h) * 2.0f - 1.0f;

		float left_tex   = source_x / (float)source_w;
		float right_tex  = (source_x + width) / (float)source_w;
		float top_tex    = (source_y + height) / (float)source_h;
		float bottom_tex = source_y / (float)source_h;

		// clang-format off
		data[ 0] = left_pos;  data[ 1] = bottom_pos; data[ 2] = left_tex;  data[ 3] = bottom_tex;
		data[ 4] = left_pos;  data[ 5] = top_pos;    data[ 6] = left_tex;  data[ 7] = top_tex;
		data[ 8] = right_pos; data[ 9] = top_pos;    data[10] = right_tex; data[11] = top_tex;
		data[12] = right_pos; data[13] = bottom_pos; data[14] = right_tex; data[15] = bottom_tex;
		// clang-format on
#ifdef KORE_WEBGL
		glBufferData(GL_ARRAY_BUFFER, 4 * 4 * 4, data, GL_STATIC_DRAW);
#else
		glUnmapBuffer(GL_ARRAY_BUFFER);
#endif
	}

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, false, 4 * 4, (void *)(int64_t)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, 4 * 4, (void *)(int64_t)(2 * 4));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, copy_index_buffer);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, source_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

	kore_opengl_check_errors();

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
