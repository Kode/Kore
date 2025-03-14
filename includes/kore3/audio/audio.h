#ifndef KORE_AUDIO_HEADER
#define KORE_AUDIO_HEADER

#include <kore3/global.h>

#include <stdint.h>

/*! \file audio.h
    \brief This is a low-level audio-API that allows you to directly provide a stream of audio-samples.
*/

#ifdef __cplusplus
extern "C" {
#endif

#define KORE_AUDIO_MAX_CHANNELS 8

typedef struct kore_audio_buffer {
	uint8_t  channel_count;
	float   *channels[KORE_AUDIO_MAX_CHANNELS];
	uint32_t data_size;
	uint32_t read_location;
	uint32_t write_location;
} kore_audio_buffer;

/// <summary>
/// Initializes the Audio2-API.
/// </summary>
KORE_FUNC void kore_audio_init(void);

/// <summary>
/// Sets the callback that's used to provide audio-samples. This is the primary method of operation for Audio2. The callback is expected to write the requested
/// number of samples into the ring-buffer. The callback is typically called from the system's audio-thread to minimize audio-latency.
/// </summary>
/// <param name="kore_audio_callback">The callback to set</param>
/// <param name="userdata">The user data provided to the callback</param>
KORE_FUNC void kore_audio_set_callback(void (*kore_audio_callback)(kore_audio_buffer *buffer, uint32_t samples, void *userdata), void *userdata);

/// <summary>
/// The current sample-rate of the system.
/// </summary>
KORE_FUNC uint32_t kore_audio_samples_per_second(void);

/// <summary>
/// Sets a callback that's called when the system's sample-rate changes.
/// </summary>
/// <param name="kore_audio_sample_rate_callback">The callback to set</param>
/// <param name="userdata">The user data provided to the callback</param>
/// <returns></returns>
KORE_FUNC void kore_audio_set_sample_rate_callback(void (*kore_audio_sample_rate_callback)(void *userdata), void *userdata);

/// <summary>
/// kore_audio_update should be called every frame. It is required by some systems to pump their audio-loops but on most systems it is a no-op.
/// </summary>
KORE_FUNC void kore_audio_update(void);

/// <summary>
/// Shuts down the Audio2-API.
/// </summary>
KORE_FUNC void kore_audio_shutdown(void);

void kore_audio_internal_init(void);
bool kore_audio_internal_callback(kore_audio_buffer *buffer, int samples);
void kore_audio_internal_sample_rate_callback(void);

#ifdef __cplusplus
}
#endif

#endif
