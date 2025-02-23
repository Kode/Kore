#include <kore3/mixer/soundstream.h>

#define STB_VORBIS_HEADER_ONLY
#include <kore3/libs/stb_vorbis.h>

#include <kore3/io/filereader.h>

#include <stdlib.h>
#include <string.h>

static kore_mixer_sound_stream streams[256];
static int nextStream = 0;
static uint8_t buffer[1024 * 1024 * 10];
static int bufferIndex;

kore_mixer_sound_stream *kore_mixer_sound_stream_create(const char *filename, bool looping) {
	kore_mixer_sound_stream *stream = &streams[nextStream];
	stream->myLooping = looping;
	stream->myVolume = 1;
	stream->rateDecodedHack = false;
	stream->end = false;
	kore_file_reader file;
	kore_file_reader_open(&file, filename, KORE_FILE_TYPE_ASSET);
	stream->buffer = &buffer[bufferIndex];
	bufferIndex += (int)kore_file_reader_size(&file);
	uint8_t *filecontent = (uint8_t *)malloc(kore_file_reader_size(&file));
	kore_file_reader_read(&file, filecontent, kore_file_reader_size(&file));
	kore_file_reader_close(&file);
	memcpy(stream->buffer, filecontent, kore_file_reader_size(&file));
	free(filecontent);
	stream->vorbis = stb_vorbis_open_memory(buffer, (int)kore_file_reader_size(&file), NULL, NULL);
	if (stream->vorbis != NULL) {
		stb_vorbis_info info = stb_vorbis_get_info(stream->vorbis);
		stream->chans = info.channels;
		stream->rate = info.sample_rate;
	}
	else {
		stream->chans = 2;
		stream->rate = 22050;
	}
	++nextStream;
	return stream;
}

int kore_mixer_sound_stream_channels(kore_mixer_sound_stream *stream) {
	return stream->chans;
}

int kore_mixer_sound_stream_sample_rate(kore_mixer_sound_stream *stream) {
	return stream->rate;
}

bool kore_mixer_sound_stream_looping(kore_mixer_sound_stream *stream) {
	return stream->myLooping;
}

void kore_mixer_sound_stream_set_looping(kore_mixer_sound_stream *stream, bool loop) {
	stream->myLooping = loop;
}

float kore_mixer_sound_stream_volume(kore_mixer_sound_stream *stream) {
	return stream->myVolume;
}

void kore_mixer_sound_stream_set_volume(kore_mixer_sound_stream *stream, float value) {
	stream->myVolume = value;
}

bool kore_mixer_sound_stream_ended(kore_mixer_sound_stream *stream) {
	return stream->end;
}

float kore_mixer_sound_stream_length(kore_mixer_sound_stream *stream) {
	if (stream->vorbis == NULL)
		return 0;
	return stb_vorbis_stream_length_in_seconds(stream->vorbis);
}

float kore_mixer_sound_stream_position(kore_mixer_sound_stream *stream) {
	if (stream->vorbis == NULL)
		return 0;
	return stb_vorbis_get_sample_offset(stream->vorbis) / stb_vorbis_stream_length_in_samples(stream->vorbis) * kore_mixer_sound_stream_length(stream);
}

void kore_mixer_sound_stream_reset(kore_mixer_sound_stream *stream) {
	if (stream->vorbis != NULL)
		stb_vorbis_seek_start(stream->vorbis);
	stream->end = false;
	stream->rateDecodedHack = false;
}

float *kore_mixer_sound_stream_next_frame(kore_mixer_sound_stream *stream) {
	if (stream->vorbis == NULL) {
		for (int i = 0; i < stream->chans; ++i) {
			stream->samples[i] = 0;
		}
		return stream->samples;
	}
	if (stream->rate == 22050) {
		if (stream->rateDecodedHack) {
			stream->rateDecodedHack = false;
			return stream->samples;
		}
	}

	float left, right;
	float *samples_array[2] = {&left, &right};
	int read = stb_vorbis_get_samples_float(stream->vorbis, stream->chans, samples_array, 1);
	if (read == 0) {
		if (kore_mixer_sound_stream_looping(stream)) {
			stb_vorbis_seek_start(stream->vorbis);
			stb_vorbis_get_samples_float(stream->vorbis, stream->chans, samples_array, 1);
		}
		else {
			stream->end = true;
			for (int i = 0; i < stream->chans; ++i) {
				stream->samples[i] = 0;
			}
			return stream->samples;
		}
	}

	stream->samples[0] = samples_array[0][0];
	stream->samples[1] = samples_array[1][0];

	stream->rateDecodedHack = true;
	return stream->samples;
}
