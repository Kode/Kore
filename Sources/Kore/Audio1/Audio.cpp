#include "Audio.h"

#include <Kore/Audio2/Audio.h>
#include <Kore/Math/Core.h>
#include <Kore/Threads/Mutex.h>
#if 0
#include <xmmintrin.h>
#endif
#include <kinc/backend/video.h>

using namespace Kore;

namespace {
	Mutex mutex;

	const int channelCount = 16;
	Audio1::Channel channels[channelCount];
	Audio1::StreamChannel streams[channelCount];
	Audio1::VideoChannel videos[channelCount];

	float sampleLinear(s16 *data, float position) {
		int pos1 = (int)position;
		int pos2 = (int)(position + 1);
		float sample1 = data[pos1] / 32767.0f;
		float sample2 = data[pos2] / 32767.0f;
		float a = position - pos1;
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
}

void Audio1::mix(uint32_t samples) {
	for (uint32_t i = 0; i < samples; ++i) {
		float leftValue = 0;
		float rightValue = 0;
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
		mutex.lock();
		for (int i = 0; i < channelCount; ++i) {
			if (channels[i].sound != nullptr) {
				// value += *(s16*)&channels[i].sound->data[(int)channels[i].position] / 32767.0f * channels[i].sound->volume();
				leftValue += sampleLinear(channels[i].sound->left, channels[i].position) * channels[i].volume * channels[i].sound->volume();
				leftValue = max(min(leftValue, 1.0f), -1.0f);
				rightValue += sampleLinear(channels[i].sound->right, channels[i].position) * channels[i].volume * channels[i].sound->volume();
				rightValue = max(min(rightValue, 1.0f), -1.0f);
				channels[i].position += channels[i].pitch / channels[i].sound->sampleRatePos;
				// channels[i].position += 2;
				if (channels[i].position + 1 >= channels[i].sound->size) {
					if (channels[i].loop) {
						channels[i].position = 0;
					}
					else {
						channels[i].sound = nullptr;
					}
				}
			}
		}
		for (int i = 0; i < channelCount; ++i) {
			if (streams[i].stream != nullptr) {
				float *samples = streams[i].stream->nextFrame();
				leftValue += samples[0] * streams[i].stream->volume();
				leftValue = max(min(leftValue, 1.0f), -1.0f);
				rightValue += samples[1] * streams[i].stream->volume();
				rightValue = max(min(rightValue, 1.0f), -1.0f);
				if (streams[i].stream->ended()) {
					streams[i].stream = nullptr;
				}
			}
		}
		for (int i = 0; i < channelCount; ++i) {
			if (videos[i].stream != nullptr) {
				float *samples = kinc_internal_video_sound_stream_next_frame(videos[i].stream);
				leftValue += samples[0];
				leftValue = max(min(leftValue, 1.0f), -1.0f);
				rightValue += samples[1];
				rightValue = max(min(rightValue, 1.0f), -1.0f);
				if (kinc_internal_video_sound_stream_ended(videos[i].stream)) {
					videos[i].stream = nullptr;
				}
			}
		}
		mutex.unlock();
#endif
		Audio2::buffer.channels[0][Audio2::buffer.writeLocation] = leftValue;
		Audio2::buffer.channels[1][Audio2::buffer.writeLocation] = rightValue;
		Audio2::buffer.writeLocation += 1;
		if (Audio2::buffer.writeLocation >= Audio2::buffer.dataSize) {
			Audio2::buffer.writeLocation = 0;
		}
	}
}

void Audio1::init() {
	for (int i = 0; i < channelCount; ++i) {
		channels[i].sound = nullptr;
		channels[i].position = 0;
	}
	for (int i = 0; i < channelCount; ++i) {
		streams[i].stream = nullptr;
		streams[i].position = 0;
	}
	mutex.create();

	Audio2::init();
	Audio2::audioCallback = mix;
}

Audio1::Channel *Audio1::play(Sound *sound, bool loop, float pitch, bool unique) {
	Channel *channel = nullptr;
	mutex.lock();
	bool found = false;
	for (int i = 0; i < channelCount; ++i) {
		if (channels[i].sound == sound) {
			found = true;
			break;
		}
	}
	if (!found || !unique) {
		for (int i = 0; i < channelCount; ++i) {
			if (channels[i].sound == nullptr) {
				channels[i].sound = sound;
				channels[i].position = 0;
				channels[i].loop = loop;
				channels[i].pitch = pitch;
				channels[i].volume = 1.0f;
				channel = &channels[i];
				break;
			}
		}
	}
	mutex.unlock();
	return channel;
}

void Audio1::stop(Sound *sound) {
	mutex.lock();
	for (int i = 0; i < channelCount; ++i) {
		if (channels[i].sound == sound) {
			channels[i].sound = nullptr;
			channels[i].position = 0;
			break;
		}
	}
	mutex.unlock();
}

void Audio1::stop(Channel *channel) {
	mutex.lock();
	channel->sound = nullptr;
	channel->position = 0;
	mutex.unlock();
}

void Audio1::play(SoundStream *stream) {
	mutex.lock();

	for (int i = 0; i < channelCount; ++i) {
		if (streams[i].stream == stream) {
			streams[i].stream = nullptr;
			streams[i].position = 0;
			break;
		}
	}

	for (int i = 0; i < channelCount; ++i) {
		if (streams[i].stream == nullptr) {
			streams[i].stream = stream;
			streams[i].position = 0;
			break;
		}
	}

	mutex.unlock();
}

void Audio1::stop(SoundStream *stream) {
	mutex.lock();
	for (int i = 0; i < channelCount; ++i) {
		if (streams[i].stream == stream) {
			streams[i].stream = nullptr;
			streams[i].position = 0;
			break;
		}
	}
	mutex.unlock();
}

void Audio1::play(kinc_internal_video_sound_stream *stream) {
	mutex.lock();
	for (int i = 0; i < channelCount; ++i) {
		if (videos[i].stream == nullptr) {
			videos[i].stream = stream;
			videos[i].position = 0;
			break;
		}
	}
	mutex.unlock();
}

void Audio1::stop(kinc_internal_video_sound_stream *stream) {
	mutex.lock();
	for (int i = 0; i < channelCount; ++i) {
		if (videos[i].stream == stream) {
			videos[i].stream = nullptr;
			videos[i].position = 0;
			break;
		}
	}
	mutex.unlock();
}
