#include <kore3/image.h>

#ifdef KINC_LZ4X
#include <kore3/libs/lz4x.h>
#else
#include <kore3/io/lz4/lz4.h>
#endif

#include <kore3/log.h>

#include <kore3/gpu/gpu.h>
#include <kore3/io/filereader.h>
#include <kore3/math/core.h>

#include <string.h>

#define BUFFER_SIZE 4096 * 4096 * 4
static uint8_t buffer[BUFFER_SIZE];
static size_t buffer_offset = 0;
static uint8_t *last_allocated_pointer = 0;

static void *buffer_malloc(size_t size) {
	uint8_t *current = &buffer[buffer_offset];
	buffer_offset += size + sizeof(size_t);
	if (buffer_offset > BUFFER_SIZE) {
		kore_log(KORE_LOG_LEVEL_ERROR, "Not enough memory on image.c Buffer.");
		return NULL;
	}
	memcpy(current, &size, sizeof(size_t));
	last_allocated_pointer = current + sizeof(size_t);
	return current + sizeof(size_t);
}

static void *buffer_realloc(void *p, size_t size) {
	if (p == NULL) {
		return buffer_malloc(size);
	}
	uint8_t *old_pointer = (uint8_t *)p;
	size_t old_size = *(size_t *)(old_pointer - sizeof(size_t));
	if (size <= old_size) {
		return old_pointer;
	}
	else {
		if (last_allocated_pointer == old_pointer) {
			size_t last_size = &buffer[buffer_offset] - old_pointer;
			size_t size_diff = size - last_size;
			buffer_offset += size_diff + sizeof(size_t);
			return old_pointer;
		}
		uint8_t *new_pointer = (uint8_t *)buffer_malloc(size);
		if (new_pointer == NULL) {
			return NULL;
		}
		memcpy(new_pointer, old_pointer, old_size < size ? old_size : size);
		return new_pointer;
	}
}

static void buffer_free(void *p) {}

#ifndef KINC_NO_STBI_BUFFER
#define STBI_MALLOC(sz) buffer_malloc(sz)
#define STBI_REALLOC(p, newsz) buffer_realloc(p, newsz)
#define STBI_FREE(p) buffer_free(p)
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#ifdef __clang__
#pragma clang diagnostic ignored "-Wunused-function"
#endif
#include <kore3/libs/stb_image.h>
#include <stdio.h>
#include <string.h>

#include <stdint.h>

typedef struct {
	kore_image_read_callbacks callbacks;
	void *user_data;
} read_data;

// fill 'data' with 'size' bytes.  return number of bytes actually read
static int stb_read(void *user, char *data, int size) {
	read_data *reader = (read_data *)user;
	return (int)reader->callbacks.read(reader->user_data, data, (size_t)size);
}

// skip the next 'n' bytes, or 'unget' the last -n bytes if negative
static void stb_skip(void *user, int n) {
	read_data *reader = (read_data *)user;
	reader->callbacks.seek(reader->user_data, reader->callbacks.pos(reader->user_data) + (size_t)n);
}

// returns nonzero if we are at end of file/data
static int stb_eof(void *user) {
	read_data *reader = (read_data *)user;
	return reader->callbacks.pos(reader->user_data) == reader->callbacks.size(reader->user_data);
}

static _Bool endsWith(const char *str, const char *suffix) {
	if (str == NULL || suffix == NULL)
		return 0;
	size_t lenstr = strlen(str);
	size_t lensuffix = strlen(suffix);
	if (lensuffix > lenstr)
		return 0;
	return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

static size_t loadImageSize(kore_image_read_callbacks callbacks, void *user_data, const char *filename) {
	if (endsWith(filename, "k")) {
		uint8_t data[4];
		callbacks.read(user_data, data, 4);
		int width = kore_read_s32le(data);
		callbacks.read(user_data, data, 4);
		int height = kore_read_s32le(data);

		char fourcc[5];
		callbacks.read(user_data, fourcc, 4);
		fourcc[4] = 0;

		if (strcmp(fourcc, "LZ4 ") == 0) {
			return width * height * 4;
		}
		else if (strcmp(fourcc, "LZ4F") == 0) {
			return width * height * 16;
		}
		else if (strcmp(fourcc, "ASTC") == 0) {
			return width * height * 4; // just an upper bound
		}
		else if (strcmp(fourcc, "DXT5") == 0) {
			return width * height; // just an upper bound
		}
		else {
			kore_log(KORE_LOG_LEVEL_ERROR, "Unknown fourcc in .k file.");
			return 0;
		}
	}
	else if (endsWith(filename, "pvr")) {
		uint8_t data[4];
		callbacks.read(user_data, data, 4); // version
		callbacks.read(user_data, data, 4); // flags
		callbacks.read(user_data, data, 4); // pixelFormat1
		callbacks.read(user_data, data, 4); // colourSpace
		callbacks.read(user_data, data, 4); // channelType
		callbacks.read(user_data, data, 4);
		uint32_t hh = kore_read_u32le(data);
		callbacks.read(user_data, data, 4);
		uint32_t ww = kore_read_u32le(data);

		return ww * hh / 2;
	}
	else if (endsWith(filename, "hdr")) {
		stbi_io_callbacks stbi_callbacks;
		stbi_callbacks.eof = stb_eof;
		stbi_callbacks.read = stb_read;
		stbi_callbacks.skip = stb_skip;

		read_data reader;
		reader.callbacks = callbacks;
		reader.user_data = user_data;

		int x, y, comp;
		stbi_info_from_callbacks(&stbi_callbacks, &reader, &x, &y, &comp);
		buffer_offset = 0;
		return x * y * 16;
	}
	else {
		stbi_io_callbacks stbi_callbacks;
		stbi_callbacks.eof = stb_eof;
		stbi_callbacks.read = stb_read;
		stbi_callbacks.skip = stb_skip;

		read_data reader;
		reader.callbacks = callbacks;
		reader.user_data = user_data;

		int x, y, comp;
		stbi_info_from_callbacks(&stbi_callbacks, &reader, &x, &y, &comp);
		buffer_offset = 0;
		return x * y * 4;
	}
}

static bool loadImage(kore_image_read_callbacks callbacks, void *user_data, const char *filename, uint8_t *output, size_t *outputSize, int *width, int *height,
                      kore_image_compression *compression, kore_image_format *format, unsigned *internalFormat, uint32_t forced_stride) {
	*format = KORE_IMAGE_FORMAT_RGBA32;
	if (endsWith(filename, "k")) {
		uint8_t data[4];
		callbacks.read(user_data, data, 4);
		*width = kore_read_s32le(data);
		callbacks.read(user_data, data, 4);
		*height = kore_read_s32le(data);

		char fourcc[5];
		callbacks.read(user_data, fourcc, 4);
		fourcc[4] = 0;

		int compressedSize = (int)callbacks.size(user_data) - 12;

		if (strcmp(fourcc, "LZ4 ") == 0) {
			*compression = KORE_IMAGE_COMPRESSION_NONE;
			*internalFormat = 0;
			*outputSize = (size_t)(*width * *height * 4);
			callbacks.read(user_data, buffer, compressedSize);
			LZ4_decompress_safe((char *)buffer, (char *)output, compressedSize, (int)*outputSize);
			return true;
		}
		else if (strcmp(fourcc, "LZ4F") == 0) {
			*compression = KORE_IMAGE_COMPRESSION_NONE;
			*internalFormat = 0;
			*outputSize = (size_t)(*width * *height * 16);
			callbacks.read(user_data, buffer, compressedSize);
			LZ4_decompress_safe((char *)buffer, (char *)output, compressedSize, (int)*outputSize);
			*format = KORE_IMAGE_FORMAT_RGBA128;
			return true;
		}
		else if (strcmp(fourcc, "ASTC") == 0) {
			*compression = KORE_IMAGE_COMPRESSION_ASTC;
			*outputSize = (size_t)(*width * *height * 4);
			callbacks.read(user_data, buffer, compressedSize);
			*outputSize = LZ4_decompress_safe((char *)buffer, (char *)output, compressedSize, (int)*outputSize);

			uint8_t blockdim_x = 6;
			uint8_t blockdim_y = 6;
			*internalFormat = (blockdim_x << 8) + blockdim_y;

			return true;

			/*int index = 0;
			index += 4; // magic
			u8 blockdim_x = astcdata[index++];
			u8 blockdim_y = astcdata[index++];
			++index; // blockdim_z
			internalFormat = (blockdim_x << 8) + blockdim_y;
			u8 xsize[4];
			xsize[0] = astcdata[index++];
			xsize[1] = astcdata[index++];
			xsize[2] = astcdata[index++];
			xsize[3] = 0;
			this->width = *(unsigned*)&xsize[0];
			u8 ysize[4];
			ysize[0] = astcdata[index++];
			ysize[1] = astcdata[index++];
			ysize[2] = astcdata[index++];
			ysize[3] = 0;
			this->height = *(unsigned*)&ysize[0];
			u8 zsize[3];
			zsize[0] = astcdata[index++];
			zsize[1] = astcdata[index++];
			zsize[2] = astcdata[index++];
			u8* all = (u8*)astcdata[index];
			dataSize -= 16;
			this->data = new u8[dataSize];
			for (int i = 0; i < dataSize; ++i) {
			data[i] = all[16 + i];
			}
			free(astcdata);*/
		}
		else if (strcmp(fourcc, "DXT5") == 0) {
			*compression = KORE_IMAGE_COMPRESSION_DXT5;
			*outputSize = (size_t)(*width * *height);
			callbacks.read(user_data, buffer, compressedSize);
			*outputSize = LZ4_decompress_safe((char *)buffer, (char *)output, compressedSize, (int)*outputSize);
			*internalFormat = 0;
			return true;
		}
		else {
			kore_log(KORE_LOG_LEVEL_ERROR, "Unknown fourcc in .k file.");
			return false;
		}
	}
	else if (endsWith(filename, "pvr")) {
		uint8_t data[4];
		callbacks.read(user_data, data, 4); // version
		callbacks.read(user_data, data, 4); // flags
		callbacks.read(user_data, data, 4); // pixelFormat1
		callbacks.read(user_data, data, 4); // colourSpace
		callbacks.read(user_data, data, 4); // channelType
		callbacks.read(user_data, data, 4);
		uint32_t hh = kore_read_u32le(data);
		callbacks.read(user_data, data, 4);
		uint32_t ww = kore_read_u32le(data);
		callbacks.read(user_data, data, 4); // depth
		callbacks.read(user_data, data, 4); // numSurfaces
		callbacks.read(user_data, data, 4); // numFaces
		callbacks.read(user_data, data, 4); // mipMapCount
		callbacks.read(user_data, data, 4);

		callbacks.read(user_data, data, 4);
		uint32_t meta1fourcc = kore_read_u32le(data);
		callbacks.read(user_data, data, 4); // meta1key
		callbacks.read(user_data, data, 4); // meta1size
		callbacks.read(user_data, data, 4);
		uint32_t meta1data = kore_read_u32le(data);

		callbacks.read(user_data, data, 4);
		uint32_t meta2fourcc = kore_read_u32le(data);
		callbacks.read(user_data, data, 4); // meta2key
		callbacks.read(user_data, data, 4); // meta2size
		callbacks.read(user_data, data, 4);
		uint32_t meta2data = kore_read_u32le(data);

		int w = 0;
		int h = 0;

		if (meta1fourcc == 0)
			w = meta1data;
		if (meta1fourcc == 1)
			h = meta1data;
		if (meta2fourcc == 0)
			w = meta2data;
		if (meta2fourcc == 1)
			h = meta2data;

		*width = w;
		*height = h;
		*compression = KORE_IMAGE_COMPRESSION_PVRTC;
		*internalFormat = 0;

		*outputSize = (size_t)(ww * hh / 2);
		callbacks.read(user_data, output, *outputSize);
		return true;
	}
	else if (endsWith(filename, "hdr")) {
		*compression = KORE_IMAGE_COMPRESSION_NONE;
		*internalFormat = 0;

		stbi_io_callbacks stbi_callbacks;
		stbi_callbacks.eof = stb_eof;
		stbi_callbacks.read = stb_read;
		stbi_callbacks.skip = stb_skip;

		read_data reader;
		reader.callbacks = callbacks;
		reader.user_data = user_data;

		int comp;
		float *uncompressed = stbi_loadf_from_callbacks(&stbi_callbacks, &reader, width, height, &comp, 4);
		if (uncompressed == NULL) {
			kore_log(KORE_LOG_LEVEL_ERROR, stbi_failure_reason());
			return false;
		}
		*outputSize = (size_t)(*width * *height * 16);
		memcpy(output, uncompressed, *outputSize);
		*format = KORE_IMAGE_FORMAT_RGBA128;
		buffer_offset = 0;
		return true;
	}
	else {
		*compression = KORE_IMAGE_COMPRESSION_NONE;
		*internalFormat = 0;

		stbi_io_callbacks stbi_callbacks;
		stbi_callbacks.eof = stb_eof;
		stbi_callbacks.read = stb_read;
		stbi_callbacks.skip = stb_skip;

		read_data reader;
		reader.callbacks = callbacks;
		reader.user_data = user_data;

		int comp;
		uint8_t *uncompressed = stbi_load_from_callbacks(&stbi_callbacks, &reader, width, height, &comp, 4);
		if (uncompressed == NULL) {
			kore_log(KORE_LOG_LEVEL_ERROR, stbi_failure_reason());
			return false;
		}

		uint32_t stride = forced_stride > 0 ? forced_stride : *width * 4;

		for (int y = 0; y < *height; ++y) {
			for (int x = 0; x < *width; ++x) {
				float r = uncompressed[y * *width * 4 + x * 4 + 0] / 255.0f;
				float g = uncompressed[y * *width * 4 + x * 4 + 1] / 255.0f;
				float b = uncompressed[y * *width * 4 + x * 4 + 2] / 255.0f;
				float a = uncompressed[y * *width * 4 + x * 4 + 3] / 255.0f;
				r *= a;
				g *= a;
				b *= a;
				output[y * stride + x * 4 + 0] = (uint8_t)kore_round(r * 255.0f);
				output[y * stride + x * 4 + 1] = (uint8_t)kore_round(g * 255.0f);
				output[y * stride + x * 4 + 2] = (uint8_t)kore_round(b * 255.0f);
				output[y * stride + x * 4 + 3] = (uint8_t)kore_round(a * 255.0f);
			}
		}
		*outputSize = (size_t)(*width * *height * 4);
		buffer_offset = 0;
		return true;
	}
}

int kore_image_format_sizeof(kore_image_format format) {
	switch (format) {
	case KORE_IMAGE_FORMAT_RGBA128:
		return 16;
	case KORE_IMAGE_FORMAT_RGBA32:
	case KORE_IMAGE_FORMAT_BGRA32:
		return 4;
	case KORE_IMAGE_FORMAT_RGBA64:
		return 8;
	case KORE_IMAGE_FORMAT_A32:
		return 4;
	case KORE_IMAGE_FORMAT_A16:
		return 2;
	case KORE_IMAGE_FORMAT_GREY8:
		return 1;
	case KORE_IMAGE_FORMAT_RGB24:
		return 3;
	}
	return -1;
}

// static bool formatIsFloatingPoint(kore_image_format_t format) {
//	return format == KINC_IMAGE_FORMAT_RGBA128 || format == KINC_IMAGE_FORMAT_RGBA64 || format == KINC_IMAGE_FORMAT_A32 || format == KINC_IMAGE_FORMAT_A16;
//}

size_t kore_image_init(kore_image *image, void *memory, int width, int height, kore_image_format format) {
	return kore_image_init3d(image, memory, width, height, 1, format);
}

size_t kore_image_init3d(kore_image *image, void *memory, int width, int height, int depth, kore_image_format format) {
	image->width = width;
	image->height = height;
	image->depth = depth;
	image->format = format;
	image->compression = KORE_IMAGE_COMPRESSION_NONE;
	image->data = memory;
	return width * height * depth * kore_image_format_sizeof(format);
}

void kore_image_init_from_bytes(kore_image *image, void *data, int width, int height, kore_image_format format) {
	kore_image_init_from_bytes3d(image, data, width, height, 1, format);
}

void kore_image_init_from_bytes3d(kore_image *image, void *data, int width, int height, int depth, kore_image_format format) {
	image->width = width;
	image->height = height;
	image->depth = depth;
	image->format = format;
	image->compression = KORE_IMAGE_COMPRESSION_NONE;
	image->data = data;
}

static size_t read_callback(void *user_data, void *data, size_t size) {
	return kore_file_reader_read((kore_file_reader *)user_data, data, size);
}

static size_t size_callback(void *user_data) {
	return kore_file_reader_size((kore_file_reader *)user_data);
}

static size_t pos_callback(void *user_data) {
	return kore_file_reader_pos((kore_file_reader *)user_data);
}

static void seek_callback(void *user_data, size_t pos) {
	kore_file_reader_seek((kore_file_reader *)user_data, pos);
}

struct kore_internal_image_memory {
	uint8_t *data;
	size_t size;
	size_t offset;
};

static size_t memory_read_callback(void *user_data, void *data, size_t size) {
	struct kore_internal_image_memory *memory = (struct kore_internal_image_memory *)user_data;
	size_t read_size = memory->size - memory->offset < size ? memory->size - memory->offset : size;
	memcpy(data, &memory->data[memory->offset], read_size);
	memory->offset += read_size;
	return read_size;
}

static size_t memory_size_callback(void *user_data) {
	struct kore_internal_image_memory *memory = (struct kore_internal_image_memory *)user_data;
	return memory->size;
}

static size_t memory_pos_callback(void *user_data) {
	struct kore_internal_image_memory *memory = (struct kore_internal_image_memory *)user_data;
	return memory->offset;
}

static void memory_seek_callback(void *user_data, size_t pos) {
	struct kore_internal_image_memory *memory = (struct kore_internal_image_memory *)user_data;
	memory->offset = pos;
}

size_t kore_image_size_from_callbacks(kore_image_read_callbacks callbacks, void *user_data, const char *filename) {
	return loadImageSize(callbacks, user_data, filename);
}

size_t kore_image_size_from_file(const char *filename) {
	kore_file_reader reader;
	if (kore_file_reader_open(&reader, filename, KORE_FILE_TYPE_ASSET)) {
		kore_image_read_callbacks callbacks;
		callbacks.read = read_callback;
		callbacks.size = size_callback;
		callbacks.pos = pos_callback;
		callbacks.seek = seek_callback;

		size_t dataSize = loadImageSize(callbacks, &reader, filename);
		kore_file_reader_close(&reader);
		return dataSize;
	}
	return 0;
}

size_t kore_image_size_from_encoded_bytes(void *data, size_t data_size, const char *format) {
	kore_image_read_callbacks callbacks;
	callbacks.read = memory_read_callback;
	callbacks.size = memory_size_callback;
	callbacks.pos = memory_pos_callback;
	callbacks.seek = memory_seek_callback;

	struct kore_internal_image_memory image_memory;
	image_memory.data = (uint8_t *)data;
	image_memory.size = data_size;
	image_memory.offset = 0;

	return loadImageSize(callbacks, &image_memory, format);
}

size_t kore_image_init_from_callbacks_with_stride(kore_image *image, void *memory, kore_image_read_callbacks callbacks, void *user_data, const char *filename,
                                                  uint32_t stride) {
	size_t dataSize = 0;
	loadImage(callbacks, user_data, filename, memory, &dataSize, &image->width, &image->height, &image->compression, &image->format, &image->internal_format,
	          stride);
	image->data = memory;
	image->data_size = dataSize;
	return dataSize;
}

size_t kore_image_init_from_callbacks(kore_image *image, void *memory, kore_image_read_callbacks callbacks, void *user_data, const char *filename) {
	return kore_image_init_from_callbacks_with_stride(image, memory, callbacks, user_data, filename, 0);
}

size_t kore_image_init_from_file_with_stride(kore_image *image, void *memory, const char *filename, uint32_t stride) {
	kore_file_reader reader;
	if (kore_file_reader_open(&reader, filename, KORE_FILE_TYPE_ASSET)) {
		kore_image_read_callbacks callbacks;
		callbacks.read = read_callback;
		callbacks.size = size_callback;
		callbacks.pos = pos_callback;
		callbacks.seek = seek_callback;

		size_t dataSize = 0;
		loadImage(callbacks, &reader, filename, memory, &dataSize, &image->width, &image->height, &image->compression, &image->format, &image->internal_format,
		          stride);
		kore_file_reader_close(&reader);
		image->data = memory;
		image->data_size = dataSize;
		image->depth = 1;
		return dataSize;
	}
	return 0;
}

size_t kore_image_init_from_file(kore_image *image, void *memory, const char *filename) {
	return kore_image_init_from_file_with_stride(image, memory, filename, 0);
}

size_t kore_image_init_from_encoded_bytes_with_stride(kore_image *image, void *memory, void *data, size_t data_size, const char *format, uint32_t stride) {
	kore_image_read_callbacks callbacks;
	callbacks.read = memory_read_callback;
	callbacks.size = memory_size_callback;
	callbacks.pos = memory_pos_callback;
	callbacks.seek = memory_seek_callback;

	struct kore_internal_image_memory image_memory;
	image_memory.data = (uint8_t *)data;
	image_memory.size = data_size;
	image_memory.offset = 0;

	size_t dataSize = 0;
	loadImage(callbacks, &image_memory, format, memory, &dataSize, &image->width, &image->height, &image->compression, &image->format, &image->internal_format,
	          stride);
	image->data = memory;
	image->data_size = dataSize;
	image->depth = 1;
	return dataSize;
}

size_t kore_image_init_from_encoded_bytes(kore_image *image, void *memory, void *data, size_t data_size, const char *format) {
	return kore_image_init_from_encoded_bytes_with_stride(image, memory, data, data_size, format, 0);
}

void kore_image_destroy(kore_image *image) {
	// user has to free the data
	image->data = NULL;
}

uint32_t kore_image_at(kore_image *image, int x, int y) {
	if (image->data == NULL) {
		return 0;
	}
	else {
		return *(uint32_t *)&((uint8_t *)image->data)[image->width * kore_image_format_sizeof(image->format) * y + x * kore_image_format_sizeof(image->format)];
	}
}

void *kore_image_at_raw(kore_image *image, int x, int y) {
	if (image->data == NULL) {
		return NULL;
	}
	else {
		return &((uint8_t *)image->data)[image->width * kore_image_format_sizeof(image->format) * y + x * kore_image_format_sizeof(image->format)];
	}
}

uint8_t *kore_image_get_pixels(kore_image *image) {
	return (uint8_t *)image->data;
}
