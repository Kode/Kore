#ifndef KORE_MIXER_SOUNDSTREAM_HEADER
#define KORE_MIXER_SOUNDSTREAM_HEADER

#include <kore3/global.h>

#include <stdbool.h>
#include <stdint.h>

/*! \file soundstream.h
    \brief Sound-Streams are decoded while playing and as such are useful for large audio-files like music or speech.
*/

#ifdef __cplusplus
extern "C" {
#endif

struct stb_vorbis;

typedef struct kore_mixer_sound_stream {
	struct stb_vorbis *vorbis;
	int chans;
	int rate;
	bool myLooping;
	float myVolume;
	bool rateDecodedHack;
	bool end;
	float samples[2];
	uint8_t *buffer;
} kore_mixer_sound_stream;

/// <summary>
/// Create a sound-stream from a wav file.
/// </summary>
/// <param name="filename">A path to a wav file</param>
/// <param name="looping">Defines whether the stream will be looped automatically</param>
/// <returns>The newly created sound-stream</returns>
KORE_FUNC kore_mixer_sound_stream *kore_mixer_sound_stream_create(const char *filename, bool looping);

/// <summary>
/// Gets the next audio-frame in the stream.
/// </summary>
/// <param name="stream">The stream to extract the frame from</param>
/// <returns>The next sample</returns>
KORE_FUNC float *kore_mixer_sound_stream_next_frame(kore_mixer_sound_stream *stream);

/// <summary>
/// Gets the number of audio-channels the stream uses.
/// </summary>
/// <param name="stream">The stream to extract the number of channels from</param>
/// <returns>The number of audio-channels</returns>
KORE_FUNC int kore_mixer_sound_stream_channels(kore_mixer_sound_stream *stream);

/// <summary>
/// Gets the sample-rate used by the stream.
/// </summary>
/// <param name="stream">The stream to extract the sample-rate from</param>
/// <returns>The sample-rate of the stream</returns>
KORE_FUNC int kore_mixer_sound_stream_sample_rate(kore_mixer_sound_stream *stream);

/// <summary>
/// Returns whether the stream loops automatically.
/// </summary>
/// <param name="stream">The stream to extract the looping-information from</param>
/// <returns>Whether the stream loops</returns>
KORE_FUNC bool kore_mixer_sound_stream_looping(kore_mixer_sound_stream *stream);

/// <summary>
/// Changes whether the stream is looped automatically.
/// </summary>
/// <param name="stream">The stream to change</param>
/// <param name="loop">The new loop value to set</param>
KORE_FUNC void kore_mixer_sound_stream_set_looping(kore_mixer_sound_stream *stream, bool loop);

/// <summary>
/// Returns whether the stream finished playing.
/// </summary>
/// <param name="stream">The stream to query</param>
/// <returns>Whether the stream finished playing</returns>
KORE_FUNC bool kore_mixer_sound_stream_ended(kore_mixer_sound_stream *stream);

/// <summary>
/// Returns the length of the stream.
/// </summary>
/// <param name="stream">The stream to query</param>
/// <returns>The length of the stream in seconds</returns>
KORE_FUNC float kore_mixer_sound_stream_length(kore_mixer_sound_stream *stream);

/// <summary>
/// Returns the current playing-position of the stream.
/// </summary>
/// <param name="stream">The stream to query</param>
/// <returns>The current playing-position in seconds</returns>
KORE_FUNC float kore_mixer_sound_stream_position(kore_mixer_sound_stream *stream);

/// <summary>
/// Resets the stream to its start-position.
/// </summary>
/// <param name="stream">The stream to change</param>
KORE_FUNC void kore_mixer_sound_stream_reset(kore_mixer_sound_stream *stream);

/// <summary>
/// Gets the stream's volume-multiplicator.
/// </summary>
/// <param name="stream">The stream to query</param>
/// <returns>The volume-multiplicator</returns>
KORE_FUNC float kore_mixer_sound_stream_volume(kore_mixer_sound_stream *stream);

/// <summary>
/// Sets the stream's volume-multiplicator.
/// </summary>
/// <param name="stream">The stream to change</param>
/// <param name="value">The volume-multiplicator</param>
KORE_FUNC void kore_mixer_sound_stream_set_volume(kore_mixer_sound_stream *stream, float value);

#ifdef __cplusplus
}
#endif

#endif
