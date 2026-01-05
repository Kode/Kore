#ifndef KORE_KOMPJUTA_VIDEO_HEADER
#define KORE_KOMPJUTA_VIDEO_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int nothing;
} kore_video_impl;

typedef struct kore_internal_video_sound_stream {
	int nothing;
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
