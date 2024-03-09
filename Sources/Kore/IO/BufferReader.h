#pragma once

#include <Kore/IO/Reader.h>

namespace Kore {

	class BufferReader : public Reader {
		u8 *buffer;
		size_t bufferSize;
		size_t position;
		void *readAllBuffer;

	public:
		BufferReader(void const *buffer, size_t size);
		virtual ~BufferReader();
		size_t read(void *data, size_t size) override;
		void *readAll() override;
		size_t size() override;
		size_t pos() override;
		void seek(size_t pos) override;
	};
}
