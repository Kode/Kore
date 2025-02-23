#ifndef KORE_MIXER_HEADER
#define KORE_MIXER_HEADER

#include <kinc/global.h>

#include "sound.h"
#include "soundstream.h"

#include <stdbool.h>

/*! \file mixer.h
    \brief Mixer is a high-level audio-API that lets you directly play audio-files.
*/

#ifdef __cplusplus
extern "C" {
#endif

struct kinc_internal_video_sound_stream;

struct kore_mixer_channel;
typedef struct kore_mixer_channel kore_mixer_channel;

struct kore_mixer_stream_channel;
typedef struct kore_mixer_stream_channel kore_mixer_stream_channel;

struct kinc_internal_video_channel;
typedef struct kinc_internal_video_channel kinc_internal_video_channel_t;

/// <summary>
/// Initialize the Mixer-API.
/// </summary>
KORE_FUNC void kore_mixer_init(void);

/// <summary>
/// Plays a sound immediately.
/// </summary>
/// <param name="sound">The sound to play</param>
/// <param name="loop">Whether or not to automatically loop the sound</param>
/// <param name="pitch">Changes the pitch by providing a value that's not 1.0f</param>
/// <param name="unique">Makes sure that a sound is not played more than once at the same time</param>
/// <returns>A channel object that can be used to control the playing sound. Please be aware that NULL is returned when the maximum number of simultaneously
/// played channels was reached.</returns>
KORE_FUNC kore_mixer_channel *kore_mixer_play_sound(kore_mixer_sound *sound, bool loop, float pitch, bool unique);

/// <summary>
/// Stops the sound from playing.
/// </summary>
/// <param name="sound">The sound to stop</param>
KORE_FUNC void kore_mixer_stop_sound(kore_mixer_sound *sound);

/// <summary>
/// Starts playing a sound-stream.
/// </summary>
/// <param name="stream">The stream to play</param>
KORE_FUNC void kore_mixer_play_sound_stream(kore_mixer_sound_stream *stream);

/// <summary>
/// Stops a sound-stream from playing.
/// </summary>
/// <param name="stream">The stream to stop.</param>
KORE_FUNC void kore_mixer_stop_sound_stream(kore_mixer_sound_stream *stream);

/// <summary>
/// Gets the current volume of a channel. Only works correctly if the channel is still playing.
/// </summary>
/// <param name="channel">The channel to get the volume from</param>
/// <returns>The volume</returns>
KORE_FUNC float kore_mixer_channel_get_volume(kore_mixer_channel *channel);

/// <summary>
/// Sets the current volume of a channel. Only works correctly if the channel is still playing.
/// </summary>
/// <param name="channel">The channel to set the volume for</param>
/// <param name="volume">The volume to set</param>
/// <returns></returns>
KORE_FUNC void kore_mixer_channel_set_volume(kore_mixer_channel *channel, float volume);

/// <summary>
/// Mixes audio into the provided buffer. kore_mixer_init sets this as the audio-callback
/// but you can also call it manually to mix with your own audio. To do that,
/// first call kore_mixer_init, then call kore_audio_set_callback to set it to your own callback
/// and call kore_mixer_mix from within that callback. Please be aware that the callback
/// will run in a separate audio-thread.
/// </summary>
/// <param name="buffer">The audio-buffer to be filled</param>
/// <param name="samples">The number of samples to be filled in</param>
KORE_FUNC void kore_mixer_mix(kore_audio_buffer *buffer, uint32_t samples);

void kinc_internal_play_video_sound_stream(struct kinc_internal_video_sound_stream *stream);
void kinc_internal_stop_video_sound_stream(struct kinc_internal_video_sound_stream *stream);

#ifdef __cplusplus
}
#endif

#endif
