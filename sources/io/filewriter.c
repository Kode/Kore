#if !defined(KORE_CONSOLE)

#include <kore3/io/filewriter.h>

#include <kore3/error.h>
#include <kore3/log.h>
#include <kore3/system.h>

#include <stdio.h>
#include <string.h>

#if defined(KORE_WINDOWS)
#include <kore3/backend/windowsmini.h>
#endif

#if defined(KORE_PS4) || defined(KORE_SWITCH)
#define MOUNT_SAVES
bool mountSaveData(bool);
void unmountSaveData();
#endif

bool kore_file_writer_open(kore_file_writer *writer, const char *filepath) {
	writer->file = NULL;
	writer->mounted = false;
#ifdef MOUNT_SAVES
	if (!mountSaveData(true)) {
		return false;
	}
	writer->mounted = true;
#endif
	char path[1001];
	strcpy(path, kore_internal_save_path());
	strcat(path, filepath);

#ifdef KORE_WINDOWS
	wchar_t wpath[MAX_PATH];
	MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, MAX_PATH);
	writer->file = CreateFileW(wpath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#else
	writer->file = fopen(path, "wb");
#endif
	if (writer->file == NULL) {
		kore_log(KORE_LOG_LEVEL_WARNING, "Could not open file %s.", filepath);
		return false;
	}
	return true;
}

void kore_file_writer_close(kore_file_writer *writer) {
	if (writer->file != NULL) {
#ifdef KORE_WINDOWS
		CloseHandle(writer->file);
#else
		fclose((FILE *)writer->file);
#endif
		writer->file = NULL;
	}
#ifdef MOUNT_SAVES
	if (writer->mounted) {
		writer->mounted = false;
		unmountSaveData();
	}
#endif
}

void kore_file_writer_write(kore_file_writer *writer, void *data, int size) {
#ifdef KORE_WINDOWS
	DWORD written = 0;
	WriteFile(writer->file, data, (DWORD)size, &written, NULL);
#else
	fwrite(data, 1, size, (FILE *)writer->file);
#endif
}

#endif
