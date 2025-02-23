#ifndef KORE_IO_FILEREADER_HEADER
#define KORE_IO_FILEREADER_HEADER

#include <kore3/global.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/*! \file filereader.h
    \brief Provides an API very similar to fread and friends but handles the intricacies of where files are actually hidden on each supported system.
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef KINC_DEBUGDIR
#define KINC_DEBUGDIR "deployment"
#endif

#ifdef KINC_ANDROID
struct AAsset;
struct __sFILE;
typedef struct __sFILE FILE;
#endif

#define KORE_FILE_TYPE_ASSET 0
#define KORE_FILE_TYPE_SAVE 1

typedef struct kore_file_reader {
	void *data; // A file handle or a more complex structure
	size_t size;
	size_t offset; // Needed by some implementations

	bool (*close)(struct kore_file_reader *reader);
	size_t (*read)(struct kore_file_reader *reader, void *data, size_t size);
	size_t (*pos)(struct kore_file_reader *reader);
	bool (*seek)(struct kore_file_reader *reader, size_t pos);
} kore_file_reader;

/// <summary>
/// Opens a file for reading.
/// </summary>
/// <param name="reader">The reader to initialize for reading</param>
/// <param name="filepath">A filepath to identify a file</param>
/// <param name="type">Looks for a regular file (KINC_FILE_TYPE_ASSET) or a save-file (KINC_FILE_TYPE_SAVE)</param>
/// <returns>Whether the file could be opened</returns>
KORE_FUNC bool kore_file_reader_open(kore_file_reader *reader, const char *filepath, int type);

/// <summary>
/// Opens a memory area for reading using the file reader API.
/// </summary>
/// <param name="reader">The reader to initialize for reading</param>
/// <param name="data">A pointer to the memory area to read</param>
/// <param name="size">The size of the memory area</param>
/// <returns>This function always returns true</returns>
KORE_FUNC bool kore_file_reader_from_memory(kore_file_reader *reader, void *data, size_t size);

/// <summary>
/// Registers a file reader callback.
/// </summary>
/// <param name="callback">The function to call when opening a file</param>
KORE_FUNC void kore_file_reader_set_callback(bool (*callback)(kore_file_reader *reader, const char *filename, int type));

/// <summary>
/// Closes a file.
/// </summary>
/// <param name="reader">The file to close</param>
/// <returns>Whether the file could be closed</returns>
KORE_FUNC bool kore_file_reader_close(kore_file_reader *reader);

/// <summary>
/// Reads data from a file starting from the current reading-position and increases the reading-position accordingly.
/// </summary>
/// <param name="reader">The reader to read from</param>
/// <param name="data">A pointer to write the data to</param>
/// <param name="size">The amount of data to read in bytes</param>
/// <returns>The number of bytes that were read - can be less than size if there is not enough data in the file</returns>
KORE_FUNC size_t kore_file_reader_read(kore_file_reader *reader, void *data, size_t size);

/// <summary>
/// Figures out the size of a file.
/// </summary>
/// <param name="reader">The reader which's file-size to figure out</param>
/// <returns>The size in bytes</returns>
KORE_FUNC size_t kore_file_reader_size(kore_file_reader *reader);

/// <summary>
/// Figures out the current reading-position in the file.
/// </summary>
/// <param name="reader">The reader which's reading-position to figure out</param>
/// <returns>The current reading-position</returns>
KORE_FUNC size_t kore_file_reader_pos(kore_file_reader *reader);

/// <summary>
/// Sets the reading-position manually.
/// </summary>
/// <param name="reader">The reader which's reading-position to set</param>
/// <param name="pos">The reading-position to set</param>
/// <returns>Whether the reading position could be set</returns>
KORE_FUNC bool kore_file_reader_seek(kore_file_reader *reader, size_t pos);

/// <summary>
/// Interprets four bytes starting at the provided pointer as a little-endian float.
/// </summary>
KORE_FUNC float kore_read_f32le(uint8_t *data);

/// <summary>
/// Interprets four bytes starting at the provided pointer as a big-endian float.
/// </summary>
KORE_FUNC float kore_read_f32be(uint8_t *data);

/// <summary>
/// Interprets eight bytes starting at the provided pointer as a little-endian uint64.
/// </summary>
KORE_FUNC uint64_t kore_read_u64le(uint8_t *data);

/// <summary>
/// Interprets eight bytes starting at the provided pointer as a big-endian uint64.
/// </summary>
KORE_FUNC uint64_t kore_read_u64be(uint8_t *data);

/// <summary>
/// Interprets eight bytes starting at the provided pointer as a little-endian int64.
/// </summary>
KORE_FUNC int64_t kore_read_s64le(uint8_t *data);

/// <summary>
/// Interprets eight bytes starting at the provided pointer as a big-endian int64.
/// </summary>
KORE_FUNC int64_t kore_read_s64be(uint8_t *data);

/// <summary>
/// Interprets four bytes starting at the provided pointer as a little-endian uint32.
/// </summary>
KORE_FUNC uint32_t kore_read_u32le(uint8_t *data);

/// <summary>
/// Interprets four bytes starting at the provided pointer as a big-endian uint32.
/// </summary>
KORE_FUNC uint32_t kore_read_u32be(uint8_t *data);

/// <summary>
/// Interprets four bytes starting at the provided pointer as a little-endian int32.
/// </summary>
KORE_FUNC int32_t kore_read_s32le(uint8_t *data);

/// <summary>
/// Interprets four bytes starting at the provided pointer as a big-endian int32.
/// </summary>
KORE_FUNC int32_t kore_read_s32be(uint8_t *data);

/// <summary>
/// Interprets two bytes starting at the provided pointer as a little-endian uint16.
/// </summary>
KORE_FUNC uint16_t kore_read_u16le(uint8_t *data);

/// <summary>
/// Interprets two bytes starting at the provided pointer as a big-endian uint16.
/// </summary>
KORE_FUNC uint16_t kore_read_u16be(uint8_t *data);

/// <summary>
/// Interprets two bytes starting at the provided pointer as a little-endian int16.
/// </summary>
KORE_FUNC int16_t kore_read_s16le(uint8_t *data);

/// <summary>
/// Interprets two bytes starting at the provided pointer as a big-endian int16.
/// </summary>
KORE_FUNC int16_t kore_read_s16be(uint8_t *data);

/// <summary>
/// Interprets one byte starting at the provided pointer as a uint8.
/// </summary>
KORE_FUNC uint8_t kore_read_u8(uint8_t *data);

/// <summary>
/// Interprets one byte starting at the provided pointer as an int8.
/// </summary>
KORE_FUNC int8_t kore_read_s8(uint8_t *data);

void kore_internal_set_files_location(char *dir);
char *kore_internal_get_files_location(void);
bool kore_internal_file_reader_callback(kore_file_reader *reader, const char *filename, int type);
bool kore_internal_file_reader_open(kore_file_reader *reader, const char *filename, int type);

#ifdef __cplusplus
}
#endif

#endif
