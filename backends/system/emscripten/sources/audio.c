#include <kore3/audio/audio.h>

#include <AL/al.h>
#include <AL/alc.h>
#include <emscripten.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static kore_audio_buffer audio_buffer;

static ALCdevice   *device   = NULL;
static ALCcontext  *context  = NULL;
static unsigned int channels = 0;
static unsigned int bits     = 0;
static ALenum       format   = 0;
static ALuint       source   = 0;

static bool audioRunning = false;
#define BUFSIZE 4096
static short buf[BUFSIZE];
#define NUM_BUFFERS 3

static uint32_t samples_per_second = 44100;

static void copySample(void *buffer) {
	float left_value  = *(float *)&audio_buffer.channels[0][audio_buffer.read_location];
	float right_value = *(float *)&audio_buffer.channels[1][audio_buffer.read_location];
	audio_buffer.read_location += 1;
	if (audio_buffer.read_location >= audio_buffer.data_size) {
		audio_buffer.read_location = 0;
	}
	((int16_t *)buffer)[0] = (int16_t)(left_value * 32767);
	((int16_t *)buffer)[1] = (int16_t)(right_value * 32767);
}

static void streamBuffer(ALuint buffer) {
	if (kore_audio_internal_callback(&audio_buffer, BUFSIZE / 2)) {
		for (int i = 0; i < BUFSIZE; i += 2) {
			copySample(&buf[i]);
		}
	}

	alBufferData(buffer, format, buf, BUFSIZE * 2, samples_per_second);
}

static void iter() {
	if (!audioRunning) {
		return;
	}

	ALint processed;
	alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

	if (processed <= 0) {
		return;
	}
	while (processed--) {
		ALuint buffer;
		alSourceUnqueueBuffers(source, 1, &buffer);
		streamBuffer(buffer);
		alSourceQueueBuffers(source, 1, &buffer);
	}

	ALint playing;
	alGetSourcei(source, AL_SOURCE_STATE, &playing);
	if (playing != AL_PLAYING) {
		alSourcePlay(source);
	}
}

static bool a2_initialized = false;

void kore_audio_init() {
	if (a2_initialized) {
		return;
	}

	kore_audio_internal_init();
	a2_initialized = true;

	audio_buffer.read_location  = 0;
	audio_buffer.write_location = 0;
	audio_buffer.data_size      = 128 * 1024;
	audio_buffer.channel_count  = 2;
	audio_buffer.channels[0]    = (float *)malloc(audio_buffer.data_size * sizeof(float));
	audio_buffer.channels[1]    = (float *)malloc(audio_buffer.data_size * sizeof(float));

	audioRunning = true;

	device  = alcOpenDevice(NULL);
	context = alcCreateContext(device, NULL);
	alcMakeContextCurrent(context);
	format = AL_FORMAT_STEREO16;

	ALuint buffers[NUM_BUFFERS];
	alGenBuffers(NUM_BUFFERS, buffers);
	alGenSources(1, &source);

	streamBuffer(buffers[0]);
	streamBuffer(buffers[1]);
	streamBuffer(buffers[2]);

	alSourceQueueBuffers(source, NUM_BUFFERS, buffers);

	alSourcePlay(source);
}

void kore_audio_update() {
	iter();
}

void kore_audio_shutdown() {
	audioRunning = false;
}

uint32_t kore_audio_samples_per_second(void) {
	return samples_per_second;
}
