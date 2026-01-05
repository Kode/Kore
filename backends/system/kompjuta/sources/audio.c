#include <kore3/audio/audio.h>

#include <stdlib.h>

static kore_audio_buffer audio_buffer;

void kore_audio_init() {
	kore_audio_internal_init();
}

void kore_audio_update() {}

void kore_audio_shutdown() {}

static uint32_t samples_per_second = 44100;

uint32_t kore_audio_samples_per_second(void) {
	return samples_per_second;
}
