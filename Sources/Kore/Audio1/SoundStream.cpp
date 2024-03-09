#include "SoundStream.h"

#define STB_VORBIS_HEADER_ONLY
#include <Kore/IO/FileReader.h>
#include <kinc/libs/stb_vorbis.c>
#include <string.h>

using namespace Kore;

SoundStream::SoundStream(const char *filename, bool looping) : myLooping(looping), myVolume(1), rateDecodedHack(false), end(false) {
	FileReader file(filename);
	buffer = new u8[file.size()];
	u8 *filecontent = (u8 *)file.readAll();
	memcpy(buffer, filecontent, file.size());
	vorbis = stb_vorbis_open_memory(buffer, (int)file.size(), nullptr, nullptr);
	if (vorbis != nullptr) {
		stb_vorbis_info info = stb_vorbis_get_info(vorbis);
		chans = info.channels;
		rate = info.sample_rate;
	}
	else {
		chans = 2;
		rate = 22050;
	}
}

int SoundStream::channels() {
	return chans;
}

int SoundStream::sampleRate() {
	return rate;
}

bool SoundStream::looping() {
	return myLooping;
}

void SoundStream::setLooping(bool loop) {
	myLooping = loop;
}

float SoundStream::volume() {
	return myVolume;
}

void SoundStream::setVolume(float value) {
	myVolume = value;
}

bool SoundStream::ended() {
	return end;
}

float SoundStream::length() {
	if (vorbis == nullptr) return 0;
	return stb_vorbis_stream_length_in_seconds(vorbis);
}

float SoundStream::position() {
	if (vorbis == nullptr) return 0;
	return stb_vorbis_get_sample_offset(vorbis) / stb_vorbis_stream_length_in_samples(vorbis) * length();
}

void SoundStream::reset() {
	if (vorbis != nullptr) stb_vorbis_seek_start(vorbis);
	end = false;
	rateDecodedHack = false;
}

float *SoundStream::nextFrame() {
	if (vorbis == nullptr) {
		for (int i = 0; i < chans; ++i) {
			samples[i] = 0;
		}
		return samples;
	}
	if (rate == 22050) {
		if (rateDecodedHack) {
			rateDecodedHack = false;
			return samples;
		}
	}

	float left, right;
	float *samplesArray[2] = {&left, &right};
	int read = stb_vorbis_get_samples_float(vorbis, chans, samplesArray, 1);
	if (read == 0) {
		if (looping()) {
			stb_vorbis_seek_start(vorbis);
			stb_vorbis_get_samples_float(vorbis, chans, samplesArray, 1);
		}
		else {
			end = true;
			for (int i = 0; i < chans; ++i) {
				samples[i] = 0;
			}
			return samples;
		}
	}

	samples[0] = samplesArray[0][0];
	samples[1] = samplesArray[1][0];

	rateDecodedHack = true;
	return samples;
}
