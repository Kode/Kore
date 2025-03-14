#include <kore3/mixer/sound.h>

#include <kore3/libs/stb_vorbis.h>

#include <kore3/audio/audio.h>
#include <kore3/error.h>
#include <kore3/io/filereader.h>

#include <assert.h>
#include <string.h>

struct WaveData {
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t bytesPerSecond;
	uint16_t bitsPerSample;
	uint32_t dataSize;
	uint8_t *data;
};

static void checkFOURCC(uint8_t **data, const char *fourcc) {
	for (int i = 0; i < 4; ++i) {
		kore_affirm(**data == fourcc[i]);
		++*data;
	}
}

static void readFOURCC(uint8_t **data, char *fourcc) {
	for (int i = 0; i < 4; ++i) {
		fourcc[i] = **data;
		++*data;
	}
	fourcc[4] = 0;
}

static void readChunk(uint8_t **data, struct WaveData *wave) {
	char fourcc[5];
	readFOURCC(data, fourcc);
	uint32_t chunksize = kore_read_u32le(*data);
	*data += 4;
	if (strcmp(fourcc, "fmt ") == 0) {
		wave->audioFormat    = kore_read_u16le(*data + 0);
		wave->numChannels    = kore_read_u16le(*data + 2);
		wave->sampleRate     = kore_read_u32le(*data + 4);
		wave->bytesPerSecond = kore_read_u32le(*data + 8);
		wave->bitsPerSample  = kore_read_u16le(*data + 14);
		*data += chunksize;
	}
	else if (strcmp(fourcc, "data") == 0) {
		wave->dataSize = chunksize;
		wave->data     = (uint8_t *)malloc(chunksize * sizeof(uint8_t));
		kore_affirm(wave->data != NULL);
		memcpy(wave->data, *data, chunksize);
		*data += chunksize;
	}
	else {
		*data += chunksize;
	}
}

static int16_t convert8to16(uint8_t sample) {
	return (sample - 127) << 8;
}

static void splitStereo8(uint8_t *data, int size, int16_t *left, int16_t *right) {
	for (int i = 0; i < size; ++i) {
		left[i]  = convert8to16(data[i * 2 + 0]);
		right[i] = convert8to16(data[i * 2 + 1]);
	}
}

static void splitStereo16(int16_t *data, int size, int16_t *left, int16_t *right) {
	for (int i = 0; i < size; ++i) {
		left[i]  = data[i * 2 + 0];
		right[i] = data[i * 2 + 1];
	}
}

static void splitMono8(uint8_t *data, int size, int16_t *left, int16_t *right) {
	for (int i = 0; i < size; ++i) {
		left[i]  = convert8to16(data[i]);
		right[i] = convert8to16(data[i]);
	}
}

static void splitMono16(int16_t *data, int size, int16_t *left, int16_t *right) {
	for (int i = 0; i < size; ++i) {
		left[i]  = data[i];
		right[i] = data[i];
	}
}

#define MAXIMUM_SOUNDS 4096
static kore_mixer_sound sounds[MAXIMUM_SOUNDS] = {0};

static kore_mixer_sound *find_sound(void) {
	for (int i = 0; i < MAXIMUM_SOUNDS; ++i) {
		if (!sounds[i].in_use) {
			return &sounds[i];
		}
	}
	return NULL;
}

kore_mixer_sound *kore_a1_sound_create_from_buffer(uint8_t *audio_data, const uint32_t size, kore_mixer_audioformat format) {
	kore_mixer_sound *sound = find_sound();
	assert(sound != NULL);
	sound->in_use = true;
	sound->volume = 1.0f;
	sound->size   = 0;
	sound->left   = NULL;
	sound->right  = NULL;
	// size_t filenameLength = strlen(filename);
	uint8_t *data = NULL;

	if (format == KORE_MIXER_AUDIOFORMAT_OGG) {
		int channels, sample_rate;
		int samples = stb_vorbis_decode_memory(audio_data, size, &channels, &sample_rate, (short **)&data);
		kore_affirm(samples > 0);
		sound->channel_count      = (uint8_t)channels;
		sound->samples_per_second = (uint32_t)sample_rate;
		sound->size               = samples * 2 * sound->channel_count;
		sound->bits_per_sample    = 16;
	}
	else if (format == KORE_MIXER_AUDIOFORMAT_WAV) {
		struct WaveData wave = {0};
		{
			uint8_t *data = audio_data;

			checkFOURCC(&data, "RIFF");
			uint32_t filesize = kore_read_u32le(data);
			data += 4;
			checkFOURCC(&data, "WAVE");
			while (data + 8 - audio_data < (intptr_t)filesize) {
				readChunk(&data, &wave);
			}
		}

		sound->bits_per_sample    = (uint8_t)wave.bitsPerSample;
		sound->channel_count      = (uint8_t)wave.numChannels;
		sound->samples_per_second = wave.sampleRate;
		data                      = wave.data;
		sound->size               = wave.dataSize;
	}
	else {
		kore_affirm(false);
	}

	if (sound->channel_count == 1) {
		if (sound->bits_per_sample == 8) {
			sound->left  = (int16_t *)malloc(sound->size * sizeof(int16_t));
			sound->right = (int16_t *)malloc(sound->size * sizeof(int16_t));
			splitMono8(data, sound->size, sound->left, sound->right);
		}
		else if (sound->bits_per_sample == 16) {
			sound->size /= 2;
			sound->left  = (int16_t *)malloc(sound->size * sizeof(int16_t));
			sound->right = (int16_t *)malloc(sound->size * sizeof(int16_t));
			splitMono16((int16_t *)data, sound->size, sound->left, sound->right);
		}
		else {
			kore_affirm(false);
		}
	}
	else {
		// Left and right channel are in s16 audio stream, alternating.
		if (sound->bits_per_sample == 8) {
			sound->size /= 2;
			sound->left  = (int16_t *)malloc(sound->size * sizeof(int16_t));
			sound->right = (int16_t *)malloc(sound->size * sizeof(int16_t));
			splitStereo8(data, sound->size, sound->left, sound->right);
		}
		else if (sound->bits_per_sample == 16) {
			sound->size /= 4;
			sound->left  = (int16_t *)malloc(sound->size * sizeof(int16_t));
			sound->right = (int16_t *)malloc(sound->size * sizeof(int16_t));
			splitStereo16((int16_t *)data, sound->size, sound->left, sound->right);
		}
		else {
			kore_affirm(false);
		}
	}
	sound->sample_rate_pos = kore_audio_samples_per_second() / (float)sound->samples_per_second;
	free(data);

	return sound;
}

kore_mixer_sound *kore_a1_sound_create(const char *filename) {
	size_t filenameLength = strlen(filename);

	kore_mixer_audioformat fileformat;
	if (strncmp(&filename[filenameLength - 4], ".ogg", 4) == 0) {
		fileformat = KORE_MIXER_AUDIOFORMAT_OGG;
	}
	else if (strncmp(&filename[filenameLength - 4], ".wav", 4) == 0) {
		fileformat = KORE_MIXER_AUDIOFORMAT_WAV;
	}
	else {
		assert(false);
	}

	kore_file_reader file;
	if (!kore_file_reader_open(&file, filename, KORE_FILE_TYPE_ASSET)) {
		return NULL;
	}

	uint8_t *filedata = (uint8_t *)malloc(kore_file_reader_size(&file));
	kore_file_reader_read(&file, filedata, kore_file_reader_size(&file));
	kore_file_reader_close(&file);
	size_t filesize = kore_file_reader_size(&file);

	kore_mixer_sound *sound = kore_a1_sound_create_from_buffer(filedata, (uint32_t)filesize, fileformat);

	free(filedata);

	return sound;
}

void kore_a1_sound_destroy(kore_mixer_sound *sound) {
	free(sound->left);
	free(sound->right);
	sound->left   = NULL;
	sound->right  = NULL;
	sound->in_use = false;
}

float kore_a1_sound_volume(kore_mixer_sound *sound) {
	return sound->volume;
}

void kore_a1_sound_set_volume(kore_mixer_sound *sound, float value) {
	sound->volume = value;
}
