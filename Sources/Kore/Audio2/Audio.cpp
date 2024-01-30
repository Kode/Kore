#include "Audio.h"

#include <stdio.h>

using namespace Kore;

void (*Audio2::audioCallback)(uint32_t samples) = nullptr;
Audio2::Buffer Audio2::buffer;

namespace {
	void audio(kinc_a2_buffer_t *buffer, uint32_t samples, void *userdata) {
		if (Audio2::audioCallback != nullptr) {
			Audio2::audioCallback(samples);
		}
		else {
			for (uint32_t i = 0; i < samples; ++i) {
				Audio2::buffer.channels[0][Audio2::buffer.writeLocation] = 0.0f;
				Audio2::buffer.channels[1][Audio2::buffer.writeLocation] = 0.0f;
				Audio2::buffer.writeLocation += 1;
				if (Audio2::buffer.writeLocation >= Audio2::buffer.dataSize) {
					Audio2::buffer.writeLocation = 0;
				}
			}
		}
		for (uint32_t i = 0; i < samples; ++i) {
			float leftSample = Audio2::buffer.channels[0][Audio2::buffer.readLocation];
			float rightSample = Audio2::buffer.channels[1][Audio2::buffer.readLocation];
			Audio2::buffer.readLocation += 1;
			if (Audio2::buffer.readLocation >= Audio2::buffer.dataSize) {
				Audio2::buffer.readLocation = 0;
			}

			buffer->channels[0][buffer->write_location] = leftSample;
			buffer->channels[1][buffer->write_location] = rightSample;
			buffer->write_location += 1;
			if (buffer->write_location >= buffer->data_size) {
				buffer->write_location = 0;
			}
		}
	}
}

void Audio2::init() {
	kinc_a2_init();
	buffer.readLocation = 0;
	buffer.writeLocation = 0;
	buffer.dataSize = 128 * 1024;
	buffer.channelCount = 2;
	buffer.channels[0] = new float[buffer.dataSize];
	buffer.channels[1] = new float[buffer.dataSize];
	kinc_a2_set_callback(audio, nullptr);
}

void Audio2::update() {
	kinc_a2_update();
}

void Audio2::shutdown() {
	kinc_a2_shutdown();
}

uint32_t Audio2::samplesPerSecond() {
	return kinc_a2_samples_per_second();
}
