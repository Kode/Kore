#pragma once

#include <kore3/gpu/texture.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	void *assetReader;
	void *videoTrackOutput;
	void *audioTrackOutput;
	double start;
	double next;
	// double audioTime;
	unsigned long long audioTime;
	bool playing;
	void *sound;
	void *androidVideo;
	int id;
	kore_gpu_texture image;
	double lastTime;
	int myWidth;
	int myHeight;
} kore_video_impl;

typedef struct kore_internal_video_sound_stream {
	void *audioTrackOutput;
	float *buffer;
	int bufferSize;
	int bufferWritePosition;
	int bufferReadPosition;
	uint64_t read;
	uint64_t written;
} kore_internal_video_sound_stream;

void kore_internal_video_sound_stream_init(kore_internal_video_sound_stream *stream, int channel_count, int frequency);

void kore_internal_video_sound_stream_destroy(kore_internal_video_sound_stream *stream);

void kore_internal_video_sound_stream_insert_data(kore_internal_video_sound_stream *stream, float *data, int sample_count);

float *kore_internal_video_sound_stream_next_frame(kore_internal_video_sound_stream *stream);

bool kore_internal_video_sound_stream_ended(kore_internal_video_sound_stream *stream);

#ifdef __cplusplus
}
#endif
