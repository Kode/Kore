#pragma once

#include <kinc/io/filereader.h>

#include "Reader.h"

namespace Kore {
	class FileReader : public Reader {
	public:
		enum FileType { Asset, Save };

		FileReader();
		FileReader(const char *filename, FileType type = Asset);
		~FileReader();
		bool open(const char *filename, FileType type = Asset);
		void close();
		size_t read(void *data, size_t size) override;
		void *readAll() override;
		size_t size() override;
		size_t pos() override;
		void seek(size_t pos) override;

		kinc_file_reader_t reader;
		FileType type;
		void *readdata;

	private:
		bool fileOpen;
	};
}
