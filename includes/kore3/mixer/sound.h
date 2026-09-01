#ifndef KORE_MIXER_SOUND_HEADER
#define KORE_MIXER_SOUND_HEADER

#include <kore3/global.h>

#include <kore3/audio/audio.h>

#include <stdint.h>

/*! \file sound.h
    \brief Sounds are pre-decoded on load and therefore primarily useful for playing sound-effects.
*/

#ifdef __cplusplus
extern "C" {
#endif

typedef struct kore_mixer_sound {
	uint8_t  channel_count;
	uint8_t  bits_per_sample;
	uint32_t samples_per_second;
	int16_t *left;
	int16_t *right;
	int      size;
	float    sample_rate_pos;
	float    volume;
	bool     in_use;
} kore_mixer_sound;

typedef enum { KORE_MIXER_AUDIOFORMAT_WAV, KORE_MIXER_AUDIOFORMAT_OGG } kore_mixer_audioformat;

/// <summary>
/// Create a sound from a wav or ogg file.
/// </summary>
/// <param name="filename">Path to a wav or ogg file</param>
/// <returns>The newly created sound</returns>
KORE_FUNC kore_mixer_sound *kore_mixer_sound_create_from_file(const char *filename);

/// <summary>
/// Create a sound from the contents of a wav or ogg file.
/// </summary>
/// <param name="audio_data">Path to ogg or mp3 data</param>
/// <param name="size">Size of the audio_data</param>
/// <param name="format">Type of the audio_data (wav or ogg)</param>
/// <returns>The newly created sound</returns>
KORE_FUNC kore_mixer_sound *kore_mixer_sound_create_from_file_buffer(uint8_t *audio_data, uint32_t size, kore_mixer_audioformat format);

/// <summary>
/// Create a sound from an array of samples.
/// </summary>
/// <param name="samples">The samples</param>
/// <param name="samples_count">Number of samples</param>
/// <param name="samples_per_second">The sample rate</param>
/// <param name="stereo">Whether the samples are interleaved stereo or mono</param>
/// <returns>The newly created sound</returns>
KORE_FUNC kore_mixer_sound *kore_mixer_sound_create_from_samples(int16_t *samples, uint32_t samples_count, uint32_t samples_per_second, bool stereo);

/// <summary>
/// Destroy a sound.
/// </summary>
/// <param name="sound">The sound to destroy.</param>
KORE_FUNC void kore_mixer_sound_destroy(kore_mixer_sound *sound);

/// <summary>
/// Gets the current volume-multiplicator that's used when playing the sound.
/// </summary>
/// <param name="sound">The sound to query</param>
/// <returns>The volume-multiplicator</returns>
KORE_FUNC float kore_mixer_sound_volume(kore_mixer_sound *sound);

/// <summary>
/// Sets the volume-multiplicator that's used when playing the sound.
/// </summary>
/// <param name="sound">The sound to modify</param>
/// <param name="value">The volume-multiplicator to set</param>
KORE_FUNC void kore_mixer_sound_set_volume(kore_mixer_sound *sound, float value);

#ifdef __cplusplus
}
#endif

#endif
