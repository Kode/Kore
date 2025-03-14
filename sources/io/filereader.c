#include <kore3/io/filereader.h>

#include <kore3/system.h>

#ifdef KORE_ANDROID
#include <kore3/backend/android.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef KORE_MICROSOFT
#include <malloc.h>
#include <memory.h>
#endif

static bool memory_close_callback(kore_file_reader *reader) {
	return true;
}

static size_t memory_read_callback(kore_file_reader *reader, void *data, size_t size) {
	size_t read_size = reader->size - reader->offset < size ? reader->size - reader->offset : size;
	memcpy(data, (uint8_t *)reader->data + reader->offset, read_size);
	reader->offset += read_size;
	return read_size;
}

static size_t memory_pos_callback(kore_file_reader *reader) {
	return reader->offset;
}

static bool memory_seek_callback(kore_file_reader *reader, size_t pos) {
	reader->offset = pos;
	return true;
}

bool kore_file_reader_from_memory(kore_file_reader *reader, void *data, size_t size) {
	memset(reader, 0, sizeof(kore_file_reader));
	reader->data  = data;
	reader->size  = size;
	reader->read  = memory_read_callback;
	reader->pos   = memory_pos_callback;
	reader->seek  = memory_seek_callback;
	reader->close = memory_close_callback;
	return true;
}

#ifdef KORE_IOS
const char *iphonegetresourcepath(void);
#endif

#ifdef KORE_MACOS
const char *macgetresourcepath(void);
#endif

#if defined(KORE_MICROSOFT)
#include <kore3/backend/windowsmini.h>
#endif

#ifdef KORE_RASPBERRY_PI
#define KORE_LINUX
#endif

static char *fileslocation                                                                    = NULL;
static bool (*file_reader_callback)(kore_file_reader *reader, const char *filename, int type) = NULL;
#ifdef KORE_MICROSOFT
static wchar_t wfilepath[1001];
#endif

void kore_internal_set_files_location(char *dir) {
	fileslocation = dir;
}

char *kore_internal_get_files_location(void) {
	return fileslocation;
}

bool kore_internal_file_reader_callback(kore_file_reader *reader, const char *filename, int type) {
	return file_reader_callback ? file_reader_callback(reader, filename, type) : false;
}

#ifdef KORE_WINDOWSAPP
void kore_internal_uwp_installed_location_path(char *path);
#endif

#if defined(KORE_MICROSOFT)
static size_t kore_libc_file_reader_read(kore_file_reader *reader, void *data, size_t size) {
	DWORD readBytes = 0;
	if (ReadFile(reader->data, data, (DWORD)size, &readBytes, NULL)) {
		return (size_t)readBytes;
	}
	else {
		return 0;
	}
}

static bool kore_libc_file_reader_seek(kore_file_reader *reader, size_t pos) {
	// TODO: make this 64-bit compliant
	SetFilePointer(reader->data, (LONG)pos, NULL, FILE_BEGIN);
	return true;
}

static bool kore_libc_file_reader_close(kore_file_reader *reader) {
	CloseHandle(reader->data);
	return true;
}

static size_t kore_libc_file_reader_pos(kore_file_reader *reader) {
	// TODO: make this 64-bit compliant
	return (size_t)SetFilePointer(reader->data, 0, NULL, FILE_CURRENT);
}
#else
static size_t kore_libc_file_reader_read(kore_file_reader *reader, void *data, size_t size) {
	return fread(data, 1, size, (FILE *)reader->data);
}

static bool kore_libc_file_reader_seek(kore_file_reader *reader, size_t pos) {
	fseek((FILE *)reader->data, pos, SEEK_SET);
	return true;
}

static bool kore_libc_file_reader_close(kore_file_reader *reader) {
	if (reader->data != NULL) {
		fclose((FILE *)reader->data);
		reader->data = NULL;
	}
	return true;
}

static size_t kore_libc_file_reader_pos(kore_file_reader *reader) {
	return ftell((FILE *)reader->data);
}
#endif

bool kore_internal_file_reader_open(kore_file_reader *reader, const char *filename, int type) {
	char filepath[1001];
#ifdef KORE_IOS
	strcpy(filepath, type == KORE_FILE_TYPE_SAVE ? kore_internal_save_path() : iphonegetresourcepath());
	if (type != KORE_FILE_TYPE_SAVE) {
		strcat(filepath, "/");
		strcat(filepath, KORE_DEBUGDIR);
		strcat(filepath, "/");
	}

	strcat(filepath, filename);
#endif
#ifdef KORE_MACOS
	strcpy(filepath, type == KORE_FILE_TYPE_SAVE ? kore_internal_save_path() : macgetresourcepath());
	if (type != KORE_FILE_TYPE_SAVE) {
		strcat(filepath, "/");
		strcat(filepath, KORE_DEBUGDIR);
		strcat(filepath, "/");
	}
	strcat(filepath, filename);
#endif
#ifdef KORE_MICROSOFT
	if (type == KORE_FILE_TYPE_SAVE) {
		strcpy(filepath, kore_internal_save_path());
		strcat(filepath, filename);
	}
	else {
		strcpy(filepath, filename);
	}
	size_t filepathlength = strlen(filepath);
	for (size_t i = 0; i < filepathlength; ++i)
		if (filepath[i] == '/')
			filepath[i] = '\\';
#endif
#ifdef KORE_WINDOWSAPP
	kore_internal_uwp_installed_location_path(filepath);
	strcat(filepath, "\\");
	strcat(filepath, filename);
#endif
#if defined(KORE_LINUX) || defined(KORE_ANDROID)
	if (type == KORE_FILE_TYPE_SAVE) {
		strcpy(filepath, kore_internal_save_path());
		strcat(filepath, filename);
	}
	else {
		strcpy(filepath, filename);
	}
#endif
#ifdef KORE_WASM
	strcpy(filepath, filename);
#endif
#ifdef KORE_EMSCRIPTEN
	strcpy(filepath, KORE_DEBUGDIR);
	strcat(filepath, "/");
	strcat(filepath, filename);
#endif

#ifdef KORE_MICROSOFT
	// Drive letter or network
	bool isAbsolute = (filename[1] == ':' && filename[2] == '\\') || (filename[0] == '\\' && filename[1] == '\\');
#else
	bool isAbsolute = filename[0] == '/';
#endif

	if (isAbsolute) {
		strcpy(filepath, filename);
	}
	else if (fileslocation != NULL && type != KORE_FILE_TYPE_SAVE) {
		strcpy(filepath, fileslocation);
		strcat(filepath, "/");
		strcat(filepath, filename);
	}

#ifdef KORE_MICROSOFT
	MultiByteToWideChar(CP_UTF8, 0, filepath, -1, wfilepath, 1000);
	reader->data = CreateFileW(wfilepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (reader->data == INVALID_HANDLE_VALUE) {
		return false;
	}
#else
	reader->data = fopen(filepath, "rb");
	if (reader->data == NULL) {
		return false;
	}
#endif

#ifdef KORE_MICROSOFT
	// TODO: make this 64-bit compliant
	reader->size = (size_t)GetFileSize(reader->data, NULL);
#else
	fseek((FILE *)reader->data, 0, SEEK_END);
	reader->size = ftell((FILE *)reader->data);
	fseek((FILE *)reader->data, 0, SEEK_SET);
#endif

	reader->read  = kore_libc_file_reader_read;
	reader->seek  = kore_libc_file_reader_seek;
	reader->close = kore_libc_file_reader_close;
	reader->pos   = kore_libc_file_reader_pos;

	return true;
}

#if !defined(KORE_ANDROID) && !defined(KORE_CONSOLE)
bool kore_file_reader_open(kore_file_reader *reader, const char *filename, int type) {
	memset(reader, 0, sizeof(*reader));
	return kore_internal_file_reader_callback(reader, filename, type) || kore_internal_file_reader_open(reader, filename, type);
}
#endif

void kore_file_reader_set_callback(bool (*callback)(kore_file_reader *reader, const char *filename, int type)) {
	file_reader_callback = callback;
}

size_t kore_file_reader_read(kore_file_reader *reader, void *data, size_t size) {
	return reader->read(reader, data, size);
}

bool kore_file_reader_seek(kore_file_reader *reader, size_t pos) {
	return reader->seek(reader, pos);
}

bool kore_file_reader_close(kore_file_reader *reader) {
	return reader->close(reader);
}

size_t kore_file_reader_pos(kore_file_reader *reader) {
	return reader->pos(reader);
}

size_t kore_file_reader_size(kore_file_reader *reader) {
	return reader->size;
}

float kore_read_f32le(uint8_t *data) {
#ifdef KORE_LITTLE_ENDIAN // speed optimization
	return *(float *)data;
#else // works on all architectures
	int i = (data[0] << 0) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
	return *(float *)&i;
#endif
}

float kore_read_f32be(uint8_t *data) {
#ifdef KORE_BIG_ENDIAN // speed optimization
	return *(float *)data;
#else // works on all architectures
	int i = (data[3] << 0) | (data[2] << 8) | (data[1] << 16) | (data[0] << 24);
	return *(float *)&i;
#endif
}

uint64_t kore_read_u64le(uint8_t *data) {
#ifdef KORE_LITTLE_ENDIAN
	return *(uint64_t *)data;
#else
	return ((uint64_t)data[0] << 0) | ((uint64_t)data[1] << 8) | ((uint64_t)data[2] << 16) | ((uint64_t)data[3] << 24) | ((uint64_t)data[4] << 32) |
	       ((uint64_t)data[5] << 40) | ((uint64_t)data[6] << 48) | ((uint64_t)data[7] << 56);
#endif
}

uint64_t kore_read_u64be(uint8_t *data) {
#ifdef KORE_BIG_ENDIAN
	return *(uint64_t *)data;
#else
	return ((uint64_t)data[7] << 0) | ((uint64_t)data[6] << 8) | ((uint64_t)data[5] << 16) | ((uint64_t)data[4] << 24) | ((uint64_t)data[3] << 32) |
	       ((uint64_t)data[2] << 40) | ((uint64_t)data[1] << 48) | ((uint64_t)data[0] << 56);
#endif
}

int64_t kore_read_s64le(uint8_t *data) {
#ifdef KORE_LITTLE_ENDIAN
	return *(int64_t *)data;
#else
	return ((int64_t)data[0] << 0) | ((int64_t)data[1] << 8) | ((int64_t)data[2] << 16) | ((int64_t)data[3] << 24) | ((int64_t)data[4] << 32) |
	       ((int64_t)data[5] << 40) | ((int64_t)data[6] << 48) | ((int64_t)data[7] << 56);
#endif
}

int64_t kore_read_s64be(uint8_t *data) {
#ifdef KORE_BIG_ENDIAN
	return *(int64_t *)data;
#else
	return ((int64_t)data[7] << 0) | ((int64_t)data[6] << 8) | ((int64_t)data[5] << 16) | ((int64_t)data[4] << 24) | ((int64_t)data[3] << 32) |
	       ((int64_t)data[2] << 40) | ((int64_t)data[1] << 48) | ((int64_t)data[0] << 56);
#endif
}

uint32_t kore_read_u32le(uint8_t *data) {
#ifdef KORE_LITTLE_ENDIAN
	return *(uint32_t *)data;
#else
	return (data[0] << 0) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
#endif
}

uint32_t kore_read_u32be(uint8_t *data) {
#ifdef KORE_BIG_ENDIAN
	return *(uint32_t *)data;
#else
	return (data[3] << 0) | (data[2] << 8) | (data[1] << 16) | (data[0] << 24);
#endif
}

int32_t kore_read_s32le(uint8_t *data) {
#ifdef KORE_LITTLE_ENDIAN
	return *(int32_t *)data;
#else
	return (data[0] << 0) | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
#endif
}

int32_t kore_read_s32be(uint8_t *data) {
#ifdef KORE_BIG_ENDIAN
	return *(int32_t *)data;
#else
	return (data[3] << 0) | (data[2] << 8) | (data[1] << 16) | (data[0] << 24);
#endif
}

uint16_t kore_read_u16le(uint8_t *data) {
#ifdef KORE_LITTLE_ENDIAN
	return *(uint16_t *)data;
#else
	return (data[0] << 0) | (data[1] << 8);
#endif
}

uint16_t kore_read_u16be(uint8_t *data) {
#ifdef KORE_BIG_ENDIAN
	return *(uint16_t *)data;
#else
	return (data[1] << 0) | (data[0] << 8);
#endif
}

int16_t kore_read_s16le(uint8_t *data) {
#ifdef KORE_LITTLE_ENDIAN
	return *(int16_t *)data;
#else
	return (data[0] << 0) | (data[1] << 8);
#endif
}

int16_t kore_read_s16be(uint8_t *data) {
#ifdef KORE_BIG_ENDIAN
	return *(int16_t *)data;
#else
	return (data[1] << 0) | (data[0] << 8);
#endif
}

uint8_t kore_read_u8(uint8_t *data) {
	return *data;
}

int8_t kore_read_s8(uint8_t *data) {
	return *(int8_t *)data;
}
