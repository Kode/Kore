#ifndef KORE_IO_FILEWRITER_HEADER
#define KORE_IO_FILEWRITER_HEADER

#include <kore3/global.h>

#include <stdbool.h>

/*! \file filewriter.h
    \brief Provides an API very similar to fwrite and friends but uses a directory that can actually used for persistent file storage. This can later be read
   using the kinc_file_reader-functions and KINC_FILE_TYPE_SAVE.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_file_writer {
	void *file;
	const char *filename;
	bool mounted;
} kore_file_writer;

/// <summary>
/// Opens a file for writing.
/// </summary>
/// <param name="reader">The writer to initialize for writing</param>
/// <param name="filepath">A filepath to identify a file</param>
/// <returns>Whether the file could be opened</returns>
KORE_FUNC bool kore_file_writer_open(kore_file_writer *writer, const char *filepath);

/// <summary>
/// Writes data to a file starting from the current writing-position and increases the writing-position accordingly.
/// </summary>
/// <param name="reader">The writer to write to</param>
/// <param name="data">A pointer to read the data from</param>
/// <param name="size">The amount of data to write in bytes</param>
KORE_FUNC void kore_file_writer_write(kore_file_writer *writer, void *data, int size);

/// <summary>
/// Closes a file.
/// </summary>
/// <param name="reader">The file to close</param>
KORE_FUNC void kore_file_writer_close(kore_file_writer *writer);

#ifdef __cplusplus
}
#endif

#endif