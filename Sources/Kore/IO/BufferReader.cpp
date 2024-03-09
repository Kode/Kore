#include "BufferReader.h"

#include <stdlib.h>
#include <string.h>

namespace Kore {
	BufferReader::BufferReader(void const *buffer, size_t size) : buffer((u8 *)buffer), bufferSize(size), position(0), readAllBuffer(nullptr) {}

	BufferReader::~BufferReader() {
		if (readAllBuffer != nullptr) free(readAllBuffer);
	}

	size_t BufferReader::read(void *data, size_t size) {
		size_t bytesAvailable = bufferSize - position;
		if (size > bytesAvailable) {
			size = bytesAvailable;
		}
		memcpy(data, buffer + position, size);
		position += size;
		return size;
	}

	// create a copy of the buffer, because returned buffer can be changed...
	void *BufferReader::readAll() {
		if (readAllBuffer != nullptr) free(readAllBuffer);
		readAllBuffer = malloc(bufferSize);
		memcpy(readAllBuffer, buffer, bufferSize);
		return readAllBuffer;
	}

	size_t BufferReader::size() {
		return bufferSize;
	}

	size_t BufferReader::pos() {
		return position;
	}

	void BufferReader::seek(size_t pos) {
		position = pos < 0 ? 0 : (pos > bufferSize ? bufferSize : pos);
	}
}
