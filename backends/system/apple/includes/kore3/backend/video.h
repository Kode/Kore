#ifndef KORE_APPLE_VIDEO_HEADER
#define KORE_APPLE_VIDEO_HEADER

#include <objc/runtime.h>

#include <kore3/gpu/texture.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	double start;
	double videoStart;
	double next;
	// double audioTime;
	unsigned long long audioTime;
	bool playing;
	bool loop;
	void *sound;
	bool image_initialized;
	kore_gpu_texture image;
	double lastTime;
	float duration;
	bool finished;
	int myWidth;
	int myHeight;

	id videoAsset;
	id assetReader;
	id videoTrackOutput;
	id audioTrackOutput;
	id url;
} kore_video_impl;

typedef struct kore_internal_video_sound_stream {
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

#endif
