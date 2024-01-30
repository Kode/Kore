#pragma once

#include <Kore/Audio2/Audio.h>
#include <Kore/IO/Reader.h>

namespace Kore {
	struct BufferFormat {
		int channels;
		int samplesPerSecond;
		int bitsPerSample;
	};

	struct Sound {
	public:
		Sound(const char *filename);
		Sound(Reader &file, const char *filename);
		~Sound();
		void load(Reader &file, const char *filename);
		BufferFormat format;
		float volume();
		void setVolume(float value);
		s16 *left;
		s16 *right;
		int size;
		float sampleRatePos;
		float length;

	private:
		float myVolume;
	};
}
