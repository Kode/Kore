#include <kore3/mixer/mixer.h>

#include <stdbool.h>

struct kore_mixer_channel {
	kore_mixer_sound *sound;
	double position;
	bool loop;
	volatile float volume;
	float pitch;
};

struct kore_mixer_stream_channel {
	kore_mixer_sound_stream *stream;
	int position;
};

struct kore_internal_video_channel {
	struct kore_internal_video_sound_stream *stream;
	int position;
};

#include "mixer.c"
#include "sound.c"
#include "soundstream.c"
