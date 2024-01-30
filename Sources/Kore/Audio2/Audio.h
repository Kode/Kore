#pragma once

#include <Kore/global.h>

#include <kinc/audio2/audio.h>

namespace Kore {
	namespace Audio2 {
		void init();
		void update();
		void shutdown();
		uint32_t samplesPerSecond();

		extern void (*audioCallback)(uint32_t samples);

		struct Buffer {
			uint8_t channelCount;
			float *channels[KINC_A2_MAX_CHANNELS];
			uint32_t dataSize;
			uint32_t readLocation;
			uint32_t writeLocation;
		};

		extern Buffer buffer;
	}
}
