#include <kore3/audio/audio.h>

#include <kinc/threads/mutex.h>
#include <memory.h>
#include <stddef.h>

static kinc_mutex_t mutex;

static void (*audio_callback)(kore_audio_buffer *buffer, uint32_t samples, void *userdata) = NULL;
static void *audio_userdata = NULL;

void kore_audio_set_callback(void (*kore_audio_callback)(kore_audio_buffer *buffer, uint32_t samples, void *userdata), void *userdata) {
	kinc_mutex_lock(&mutex);
	audio_callback = kore_audio_callback;
	audio_userdata = userdata;
	kinc_mutex_unlock(&mutex);
}

static void (*audio_sample_rate_callback)(void *userdata) = NULL;
static void *audio_sample_rate_userdata = NULL;

void kore_audio_set_sample_rate_callback(void (*kore_audio_sample_rate_callback)(void *userdata), void *userdata) {
	kinc_mutex_lock(&mutex);
	audio_sample_rate_callback = kore_audio_sample_rate_callback;
	audio_sample_rate_userdata = userdata;
	kinc_mutex_unlock(&mutex);
}

void kore_audio_internal_init(void) {
	kinc_mutex_init(&mutex);
}

bool kore_audio_internal_callback(kore_audio_buffer *buffer, int samples) {
	kinc_mutex_lock(&mutex);
	bool has_callback = audio_callback != NULL;
	if (has_callback) {
		audio_callback(buffer, samples, audio_userdata);
	}
	kinc_mutex_unlock(&mutex);
	return has_callback;
}

void kore_audio_internal_sample_rate_callback(void) {
	kinc_mutex_lock(&mutex);
	if (audio_sample_rate_callback != NULL) {
		audio_sample_rate_callback(audio_sample_rate_userdata);
	}
	kinc_mutex_unlock(&mutex);
}
