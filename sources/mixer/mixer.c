#include <kore3/mixer/mixer.h>

#include <stdint.h>

#include <kore3/audio/audio.h>
#include <kore3/math/core.h>
#include <kore3/threads/atomic.h>
#include <kore3/threads/mutex.h>
#include <kore3/video.h>

#include <assert.h>
#include <stdlib.h>

static kore_mutex mutex;

#define CHANNEL_COUNT 16
static kore_mixer_channel          channels[CHANNEL_COUNT];
static kore_mixer_stream_channel   streamchannels[CHANNEL_COUNT];
static kore_internal_video_channel videos[CHANNEL_COUNT];

static float sampleLinear(int16_t *data, double position) {
	int   pos1    = (int)position;
	int   pos2    = (int)(position + 1);
	float sample1 = data[pos1] / 32767.0f;
	float sample2 = data[pos2] / 32767.0f;
	float a       = (float)(position - pos1);
	return sample1 * (1 - a) + sample2 * a;
}

/*float sampleHermite4pt3oX(s16* data, float position) {
    float s0 = data[(int)(position - 1)] / 32767.0f;
    float s1 = data[(int)(position + 0)] / 32767.0f;
    float s2 = data[(int)(position + 1)] / 32767.0f;
    float s3 = data[(int)(position + 2)] / 32767.0f;

    float x = position - (int)(position);

    // 4-point, 3rd-order Hermite (x-form)
    float c0 = s1;
    float c1 = 0.5f * (s2 - s0);
    float c2 = s0 - 2.5f * s1 + 2 * s2 - 0.5f * s3;
    float c3 = 0.5f * (s3 - s0) + 1.5f * (s1 - s2);
    return ((c3 * x + c2) * x + c1) * x + c0;
}*/

void kore_mixer_mix(kore_audio_buffer *buffer, uint32_t samples) {
	for (uint32_t i = 0; i < samples; ++i) {
		float left_value  = 0.0f;
		float right_value = 0.0f;
#if 0
		__m128 sseSamples[4];
		for (int i = 0; i < channelCount; i += 4) {
			s16 data[4];
			for (int i2 = 0; i2 < 4; ++i2) {
				if (channels[i + i2].sound != nullptr) {
					data[i2] = *(s16*)&channels[i + i2].sound->data[channels[i + i2].position];
					channels[i + i2].position += 2;
					if (channels[i + i2].position >= channels[i + i2].sound->size) channels[i + i2].sound = nullptr;
				}
				else {
					data[i2] = 0;
				}
			}
			sseSamples[i / 4] = _mm_set_ps(data[3] / 32767.0f, data[2] / 32767.0f, data[1] / 32767.0f, data[0] / 32767.0f);
		}
		__m128 a = _mm_add_ps(sseSamples[0], sseSamples[1]);
		__m128 b = _mm_add_ps(sseSamples[2], sseSamples[3]);
		__m128 c = _mm_add_ps(a, b);
		value = c.m128_f32[0] + c.m128_f32[1] + c.m128_f32[2] + c.m128_f32[3];
		value = max(min(value, 1.0f), -1.0f);
#else
		kore_mutex_lock(&mutex);
		for (int i = 0; i < CHANNEL_COUNT; ++i) {
			if (channels[i].sound != NULL) {
				left_value += sampleLinear(channels[i].sound->left, channels[i].position) * channels[i].volume * channels[i].sound->volume;
				left_value = kore_max(kore_min(left_value, 1.0f), -1.0f);
				right_value += sampleLinear(channels[i].sound->right, channels[i].position) * channels[i].volume * channels[i].sound->volume;
				right_value = kore_max(kore_min(right_value, 1.0f), -1.0f);

				channels[i].position += channels[i].pitch / channels[i].sound->sample_rate_pos;
				// channels[i].position += 2;
				if (channels[i].position + 1 >= channels[i].sound->size) {
					if (channels[i].loop) {
						channels[i].position = 0;
					}
					else {
						channels[i].sound = NULL;
					}
				}
			}
		}
		for (int i = 0; i < CHANNEL_COUNT; ++i) {
			if (streamchannels[i].stream != NULL) {
				float *samples = kore_mixer_sound_stream_next_frame(streamchannels[i].stream);
				left_value += samples[0] * kore_mixer_sound_stream_volume(streamchannels[i].stream);
				left_value = kore_max(kore_min(left_value, 1.0f), -1.0f);
				right_value += samples[1] * kore_mixer_sound_stream_volume(streamchannels[i].stream);
				right_value = kore_max(kore_min(right_value, 1.0f), -1.0f);
				if (kore_mixer_sound_stream_ended(streamchannels[i].stream)) {
					streamchannels[i].stream = NULL;
				}
			}
		}

		for (int i = 0; i < CHANNEL_COUNT; ++i) {
			if (videos[i].stream != NULL) {
				float *samples = kore_internal_video_sound_stream_next_frame(videos[i].stream);
				left_value += samples[0];
				left_value = kore_max(kore_min(left_value, 1.0f), -1.0f);
				right_value += samples[1];
				right_value = kore_max(kore_min(right_value, 1.0f), -1.0f);
				if (kore_internal_video_sound_stream_ended(videos[i].stream)) {
					videos[i].stream = NULL;
				}
			}
		}

		kore_mutex_unlock(&mutex);
#endif
		assert(buffer->channel_count >= 2);
		buffer->channels[0][buffer->write_location] = left_value;
		buffer->channels[1][buffer->write_location] = right_value;

		buffer->write_location += 1;
		if (buffer->write_location >= buffer->data_size) {
			buffer->write_location = 0;
		}
	}
}

static void kore_mixer_mix_callback(kore_audio_buffer *buffer, uint32_t samples, void *userdata) {
	kore_mixer_mix(buffer, samples);
}

void kore_mixer_init(void) {
	for (int i = 0; i < CHANNEL_COUNT; ++i) {
		channels[i].sound    = NULL;
		channels[i].position = 0;
	}
	for (int i = 0; i < CHANNEL_COUNT; ++i) {
		streamchannels[i].stream   = NULL;
		streamchannels[i].position = 0;
	}
	kore_mutex_init(&mutex);

	kore_audio_init();
	kore_audio_set_callback(kore_mixer_mix_callback, NULL);
}

kore_mixer_channel *kore_mixer_play_sound(kore_mixer_sound *sound, bool loop, float pitch, bool unique) {
	kore_mixer_channel *channel = NULL;
	kore_mutex_lock(&mutex);
	bool found = false;
	for (int i = 0; i < CHANNEL_COUNT; ++i) {
		if (channels[i].sound == sound) {
			found = true;
			break;
		}
	}
	if (!found || !unique) {
		for (int i = 0; i < CHANNEL_COUNT; ++i) {
			if (channels[i].sound == NULL) {
				channels[i].sound    = sound;
				channels[i].position = 0;
				channels[i].loop     = loop;
				channels[i].pitch    = pitch;
				channels[i].volume   = 1.0f;
				channel              = &channels[i];
				break;
			}
		}
	}
	kore_mutex_unlock(&mutex);
	return channel;
}

void kore_mixer_stop_sound(kore_mixer_sound *sound) {
	kore_mutex_lock(&mutex);
	for (int i = 0; i < CHANNEL_COUNT; ++i) {
		if (channels[i].sound == sound) {
			channels[i].sound    = NULL;
			channels[i].position = 0;
			break;
		}
	}
	kore_mutex_unlock(&mutex);
}

void kore_mixer_play_sound_stream(kore_mixer_sound_stream *stream) {
	kore_mutex_lock(&mutex);

	for (int i = 0; i < CHANNEL_COUNT; ++i) {
		if (streamchannels[i].stream == stream) {
			streamchannels[i].stream   = NULL;
			streamchannels[i].position = 0;
			break;
		}
	}

	for (int i = 0; i < CHANNEL_COUNT; ++i) {
		if (streamchannels[i].stream == NULL) {
			streamchannels[i].stream   = stream;
			streamchannels[i].position = 0;
			break;
		}
	}

	kore_mutex_unlock(&mutex);
}

void kore_mixer_stop_sound_stream(kore_mixer_sound_stream *stream) {
	kore_mutex_lock(&mutex);
	for (int i = 0; i < CHANNEL_COUNT; ++i) {
		if (streamchannels[i].stream == stream) {
			streamchannels[i].stream   = NULL;
			streamchannels[i].position = 0;
			break;
		}
	}
	kore_mutex_unlock(&mutex);
}

void kore_internal_play_video_sound_stream(struct kore_internal_video_sound_stream *stream) {
	kore_mutex_lock(&mutex);
	for (int i = 0; i < CHANNEL_COUNT; ++i) {
		if (videos[i].stream == NULL) {
			videos[i].stream   = stream;
			videos[i].position = 0;
			break;
		}
	}
	kore_mutex_unlock(&mutex);
}

void kore_internal_stop_video_sound_stream(struct kore_internal_video_sound_stream *stream) {
	kore_mutex_lock(&mutex);
	for (int i = 0; i < CHANNEL_COUNT; ++i) {
		if (videos[i].stream == stream) {
			videos[i].stream   = NULL;
			videos[i].position = 0;
			break;
		}
	}
	kore_mutex_unlock(&mutex);
}

float kore_mixer_channel_get_volume(kore_mixer_channel *channel) {
	return channel->volume;
}

void kore_mixer_channel_set_volume(kore_mixer_channel *channel, float volume) {
	kore_atomic_exchange_float(&channel->volume, volume);
}
