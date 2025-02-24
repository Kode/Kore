#include <kore3/video.h>

#if !defined(KINC_VIDEO_GSTREAMER)
void kore_video_init(kore_video *video, const char *filename) {}

void kore_video_destroy(kore_video *video) {}

void kore_video_play(kore_video *video, bool loop) {}

void kore_video_pause(kore_video *video) {}

void kore_video_stop(kore_video *video) {}

int kore_video_width(kore_video *video) {
	return 256;
}

int kore_video_height(kore_video *video) {
	return 256;
}

kore_gpu_texture *kore_video_current_image(kore_video *video) {
	return NULL;
}

double kore_video_duration(kore_video *video) {
	return 0.0;
}

double kore_video_position(kore_video *video) {
	return 0.0;
}

bool kore_video_finished(kore_video *video) {
	return false;
}

bool kore_video_paused(kore_video *video) {
	return false;
}

void kore_video_update(kore_video *video, double time) {}

void kore_internal_video_sound_stream_init(kore_internal_video_sound_stream *stream, int channel_count, int frequency) {}

void kore_internal_video_sound_stream_destroy(kore_internal_video_sound_stream *stream) {}

void kore_internal_video_sound_stream_insert_data(kore_internal_video_sound_stream *stream, float *data, int sample_count) {}

static float samples[2] = {0};

float *kore_internal_video_sound_stream_next_frame(kore_internal_video_sound_stream *stream) {
	return samples;
}

bool kore_internal_video_sound_stream_ended(kore_internal_video_sound_stream *stream) {
	return true;
}
#endif
