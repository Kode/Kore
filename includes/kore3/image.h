#ifndef KORE_IMAGE_HEADER
#define KORE_IMAGE_HEADER

#include <kore3/global.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*! \file image.h
    \brief Functionality for creating and loading images. Image loading supports PNG, JPEG and the custom K format. K files can contain image data that uses
   texture-compression (see kore_image_compression).
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef enum kore_image_compression {
	KORE_IMAGE_COMPRESSION_NONE,
	KORE_IMAGE_COMPRESSION_DXT5,
	KORE_IMAGE_COMPRESSION_ASTC,
	KORE_IMAGE_COMPRESSION_PVRTC
} kore_image_compression;

typedef enum kore_image_format {
	KORE_IMAGE_FORMAT_RGBA32,
	KORE_IMAGE_FORMAT_GREY8,
	KORE_IMAGE_FORMAT_RGB24,
	KORE_IMAGE_FORMAT_RGBA128,
	KORE_IMAGE_FORMAT_RGBA64,
	KORE_IMAGE_FORMAT_A32,
	KORE_IMAGE_FORMAT_BGRA32,
	KORE_IMAGE_FORMAT_A16
} kore_image_format;

typedef struct kore_image {
	int width, height, depth;
	kore_image_format format;
	unsigned internal_format;
	kore_image_compression compression;
	void *data;
	size_t data_size;
} kore_image;

typedef struct kore_image_read_callbacks {
	size_t (*read)(void *user_data, void *data, size_t size);
	void (*seek)(void *user_data, size_t pos);
	size_t (*pos)(void *user_data);
	size_t (*size)(void *user_data);
} kore_image_read_callbacks;

/// <summary>
/// Creates a 2D kore_image in the provided memory.
/// </summary>
/// <returns>The size that's occupied by the image in memory in bytes</returns>
KORE_FUNC size_t kore_image_init(kore_image *image, void *memory, int width, int height, kore_image_format format);

/// <summary>
/// Creates a 3D kore_image in the provided memory.
/// </summary>
/// <returns>The size that's occupied by the image in memory in bytes</returns>
KORE_FUNC size_t kore_image_init3d(kore_image *image, void *memory, int width, int height, int depth, kore_image_format format);

/// <summary>
/// Peeks into an image file and figures out the size it will occupy in memory.
/// </summary>
/// <returns>The memory size in bytes that will be used when loading the image</returns>
KORE_FUNC size_t kore_image_size_from_file(const char *filename);

/// <summary>
/// Peeks into an image that is loaded via callback functions and figures out the size it will occupy in memory.
/// </summary>
/// <returns>The memory size in bytes that will be used when loading the image</returns>
KORE_FUNC size_t kore_image_size_from_callbacks(kore_image_read_callbacks callbacks, void *user_data, const char *format);

/// <summary>
/// Peeks into an image file that resides in memory and figures out the size it will occupy in memory once it is uncompressed.
/// </summary>
/// <param name="data">The encoded data</param>
/// <param name="data_size">The size of the encoded data</param>
/// <param name="format_hint">Something like "png" can help, it also works to just put in the filename</param>
/// <returns>The memory size in bytes that will be used when loading the image</returns>
KORE_FUNC size_t kore_image_size_from_encoded_bytes(void *data, size_t data_size, const char *format_hint);

/// <summary>
/// Loads an image from a file.
/// </summary>
/// <param name="image">The image-object to initialize with the data</param>
/// <param name="memory">The pre-allocated memory to load the image-data into</param>
/// <param name="filename">The file to load</param>
/// <returns>The memory size in bytes that will be used when loading the image</returns>
KORE_FUNC size_t kore_image_init_from_file(kore_image *image, void *memory, const char *filename);

/// <summary>
/// Loads an image from a file with an enforced stride.
/// </summary>
/// <param name="image">The image-object to initialize with the data</param>
/// <param name="memory">The pre-allocated memory to load the image-data into</param>
/// <param name="filename">The file to load</param>
/// <param name="stride">The enforced stride in bytes</param>
/// <returns>The memory size in bytes that will be used when loading the image</returns>
KORE_FUNC size_t kore_image_init_from_file_with_stride(kore_image *image, void *memory, const char *filename, uint32_t stride);

/// <summary>
/// Loads an image file using callbacks.
/// </summary>
/// <param name="image">The image-object to initialize with the data</param>
/// <param name="memory">The pre-allocated memory to load the image-data into</param>
/// <param name="callbacks">The callbacks that are used to query the data to encode</param>
/// <param name="user_data">An optional pointer that is passed to the callbacks</param>
/// <returns>The memory size in bytes that will be used when loading the image</returns>
KORE_FUNC size_t kore_image_init_from_callbacks(kore_image *image, void *memory, kore_image_read_callbacks callbacks, void *user_data, const char *format);

/// <summary>
/// Loads an image file using callbacks with an enforced stride.
/// </summary>
/// <param name="image">The image-object to initialize with the data</param>
/// <param name="memory">The pre-allocated memory to load the image-data into</param>
/// <param name="callbacks">The callbacks that are used to query the data to encode</param>
/// <param name="user_data">An optional pointer that is passed to the callbacks</param>
/// <param name="stride">The enforced stride in bytes</param>
/// <returns>The memory size in bytes that will be used when loading the image</returns>
KORE_FUNC size_t kore_image_init_from_callbacks_with_stride(kore_image *image, void *memory, kore_image_read_callbacks callbacks, void *user_data,
                                                            const char *format, uint32_t stride);

/// <summary>
/// Loads an image file from memory.
/// </summary>
/// <param name="image">The image-object to initialize with the data</param>
/// <param name="memory">The pre-allocated memory to load the image-data into</param>
/// <param name="data">The data to encode</param>
/// <param name="data_size">The size of the data to encode in bytes</param>
/// <returns>The memory size in bytes that will be used when loading the image</returns>
KORE_FUNC size_t kore_image_init_from_encoded_bytes(kore_image *image, void *memory, void *data, size_t data_size, const char *format);

/// <summary>
/// Loads an image file from memory with an enforced stride.
/// </summary>
/// <param name="image">The image-object to initialize with the data</param>
/// <param name="memory">The pre-allocated memory to load the image-data into</param>
/// <param name="data">The data to encode</param>
/// <param name="data_size">The size of the data to encode in bytes</param>
/// <param name="stride">The enforced stride in bytes</param>
/// <returns>The memory size in bytes that will be used when loading the image</returns>
KORE_FUNC size_t kore_image_init_from_encoded_bytes_with_stride(kore_image *image, void *memory, void *data, size_t data_size, const char *format,
                                                                uint32_t stride);

/// <summary>
/// Creates a 2D image from memory.
/// </summary>
KORE_FUNC void kore_image_init_from_bytes(kore_image *image, void *data, int width, int height, kore_image_format format);

/// <summary>
/// Creates a 3D image from memory.
/// </summary>
KORE_FUNC void kore_image_init_from_bytes3d(kore_image *image, void *data, int width, int height, int depth, kore_image_format format);

/// <summary>
/// Destroys an image. This does not free the user-provided memory.
/// </summary>
KORE_FUNC void kore_image_destroy(kore_image *image);

/// <summary>
/// Gets the color value of a 32 bit pixel. If this doesn't fit the format of the image please use kore_image_at_raw instead.
/// </summary>
/// <returns>One 32 bit color value</returns>
KORE_FUNC uint32_t kore_image_at(kore_image *image, int x, int y);

/// <summary>
/// Gets a pointer to the color-data of one pixel.
/// </summary>
/// <returns>A pointer to the color-data of the pixel pointed to by x and y</returns>
KORE_FUNC void *kore_image_at_raw(kore_image *image, int x, int y);

/// <summary>
/// Provides access to the image data.
/// </summary>
/// <returns>A pointer to the image data</returns>
KORE_FUNC uint8_t *kore_image_get_pixels(kore_image *image);

/// <summary>
/// Gets the size in bytes of a single pixel for a given image format.
/// </summary>
/// <returns>The size of one pixel in bytes</returns>
KORE_FUNC int kore_image_format_sizeof(kore_image_format format);

#ifdef __cplusplus
}
#endif

#endif
